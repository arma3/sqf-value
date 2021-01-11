#pragma once

#include <functional>
#include <iomanip>
#include <array>
#include <vector>
#include <iostream>
#include "value.hpp"

class test
{
private:
    std::string m_name;
    std::function<bool()> m_func_setup;
    std::function<bool()> m_func_assert;
    std::function<void()> m_func_cleanup;

    static bool default_func_setup() { return true; }
    static void default_func_cleanup() { return; }
protected:
    test(std::string name) : test(name, {}, {}, {}) { }
public:
    test(std::string name, std::function<bool()> func_assert) : test(name, default_func_setup, func_assert, default_func_cleanup) { }
    test(std::string name, std::function<bool()> func_setup, std::function<bool()> func_assert) : test(name, func_setup, func_assert, default_func_cleanup) { }
    test(std::string name, std::function<bool()> func_setup, std::function<bool()> func_assert, std::function<void()> func_cleanup) :
        m_name(name),
        m_func_setup(func_setup),
        m_func_assert(func_assert),
        m_func_cleanup(func_cleanup)
    {
    }

    std::string name() const { return m_name; }
    bool setup() const { return m_func_setup(); }
    bool assert() const { return m_func_assert(); }
    void cleanup() const { m_func_cleanup(); }
};
template<typename T>
class test_compare : public test
{
private:
    std::function<bool()> m_func_setup;
    std::function<T()> m_func_assert;
    std::function<void()> m_func_cleanup;

    static bool default_func_setup() { return true; }
    static void default_func_cleanup() { return; }

public:
    test_compare(std::string name, std::function<T()> func_assert) : test_compare(name, default_func_setup, func_assert, default_func_cleanup) { }
    test_compare(std::string name, std::function<bool()> func_setup, std::function<T()> func_assert) : test_compare(name, func_setup, func_assert, default_func_cleanup) { }
    test_compare(std::string name, std::function<bool()> func_setup, std::function<T()> func_assert, std::function<void()> func_cleanup) :
        test(name),
        m_func_setup(func_setup),
        m_func_assert(func_assert),
        m_func_cleanup(func_cleanup)
    {
    }

    bool setup() const { return m_func_setup(); }
    T assert() const { return m_func_assert(); }
    void cleanup() const { m_func_cleanup(); }
};
class tester
{
private:
    size_t m_passed_count;
    size_t m_total_count;
    std::vector<test> m_failed_tests;
    void test_passed(const test &t)
    {
        m_total_count++;
        m_passed_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " passed '" << t.name() << "'." << std::endl;
    }
    void test_failed(const test &t)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        m_failed_tests.push_back(t);
    }
    template<typename T>
    void test_failed(const test_compare<T> &t, const T &val, const T &reference)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        std::cout << "     expected: " << reference << std::endl;
        std::cout << "     got:      " << val << std::endl;
        m_failed_tests.push_back(t);
    }
    template<>
    void test_failed<std::vector<sqf::value>>(const test_compare<std::vector<sqf::value>> &t, const std::vector<sqf::value> &val, const std::vector<sqf::value> &reference)
    {
        m_total_count++;
        bool first;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        std::cout << "     expected: { ";
        first = true;
        for (auto it : reference)
        {
            if (first) { first = false; }
            else { std::cout << ", "; }
            std::cout << it.to_string();
        }
        std::cout << " }" << std::endl;
        std::cout << "     got:      { ";
        first = true;
        for (auto it : val)
        {
            if (first) { first = false; }
            else { std::cout << ", "; }
            std::cout << it.to_string();
        }
        std::cout << " }" << std::endl;
        m_failed_tests.push_back(t);
    }
    void test_failed(const test_compare<sqf::value> &t, const sqf::value &val, const sqf::value &reference)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        std::cout << "     expected: " << reference.to_string() << std::endl;
        std::cout << "     got:      " << val.to_string() << std::endl;
        m_failed_tests.push_back(t);
    }
public:
    tester() : m_passed_count(0), m_total_count(0) { }
    bool all_passed() { return m_passed_count == m_total_count; }

    template<typename T>
    void assert_equals(const T &reference, const test_compare<T> &t)
    {
        if (!t.setup()) { test_failed(t); return; }
        try
        {
            auto val = t.assert();
            if (val == reference) { test_passed(t); }
            else { test_failed(t, val, reference); }
        }
        catch (std::runtime_error e)
        {
            test_failed(t);
        }
        t.cleanup();
    }
    void assert_true(const test &t)
    {
        if (!t.setup()) { test_failed(t); return; }
        try
        {
            if (t.assert() == true) { test_passed(t); }
            else { test_failed(t); }
        }
        catch (std::runtime_error e)
        {
            test_failed(t);
        }
        t.cleanup();
    }
    void assert_false(const test &t)
    {
        if (!t.setup()) { test_failed(t); return; }
        try
        {
            if (t.assert() == false) { test_passed(t); }
            else { test_failed(t); }
        }
        catch (std::runtime_error e)
        {
            test_failed(t);
        }
        t.cleanup();
    }
    void assert_error(const test &t)
    {
        if (!t.setup()) { test_failed(t); return; }
        try
        {
            t.assert();
            test_failed(t);
        }
        catch (std::runtime_error e)
        {
            test_passed(t);
        }
        t.cleanup();
    }
};