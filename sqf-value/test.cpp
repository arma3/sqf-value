#include "value.h"
#include <functional>
#include <iomanip>
#include <array>
#include <vector>
#include <iostream>

using namespace std::string_literals;

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
    test(std::string name) : test(name, {}, {}, {}) {}
public:
    test(std::string name, std::function<bool()> func_assert) : test(name, default_func_setup, func_assert, default_func_cleanup) {}
    test(std::string name, std::function<bool()> func_setup, std::function<bool()> func_assert) : test(name, func_setup, func_assert, default_func_cleanup) {}
    test(std::string name, std::function<bool()> func_setup, std::function<bool()> func_assert, std::function<void()> func_cleanup) :
        m_name(name),
        m_func_setup(func_setup),
        m_func_assert(func_assert),
        m_func_cleanup(func_cleanup)
    {}

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
    test_compare(std::string name, std::function<T()> func_assert) : test_compare(name, default_func_setup, func_assert, default_func_cleanup) {}
    test_compare(std::string name, std::function<bool()> func_setup, std::function<T()> func_assert) : test_compare(name, func_setup, func_assert, default_func_cleanup) {}
    test_compare(std::string name, std::function<bool()> func_setup, std::function<T()> func_assert, std::function<void()> func_cleanup) :
        test(name),
        m_func_setup(func_setup),
        m_func_assert(func_assert),
        m_func_cleanup(func_cleanup)
    {}

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
    void test_passed(const test& t)
    {
        m_total_count++;
        m_passed_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " passed '" << t.name() << "'." << std::endl;
    }
    void test_failed(const test& t)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        m_failed_tests.push_back(t);
    }
    template<typename T>
    void test_failed(const test_compare<T>& t, const T& val, const T& reference)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        std::cout << "     expected: " << reference << std::endl;
        std::cout << "     got:      " << val << std::endl;
        m_failed_tests.push_back(t);
    }
    void test_failed(const test_compare<sqf::value>& t, const sqf::value& val, const sqf::value& reference)
    {
        m_total_count++;
        std::cout << "Test " << std::setfill(' ') << std::setw(4) << m_total_count << " FAILED '" << t.name() << "'." << std::endl;
        std::cout << "     expected: " << reference.to_string() << std::endl;
        std::cout << "     got:      " << val.to_string() << std::endl;
        m_failed_tests.push_back(t);
    }
public:
    tester() : m_passed_count(0), m_total_count(0) {}
    bool all_passed() { return m_passed_count == m_total_count; }

    template<typename T>
    void assert_equals(const T& reference, const test_compare<T>& t)
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
    void assert_true(const test& t)
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
    void assert_false(const test& t)
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
    void assert_error(const test& t)
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

int main()
{
    tester tester;

    tester.assert_equals("nil"s, { "ToString Test: NIL",             []() { return sqf::value().to_string(); } });
    tester.assert_equals("\"test\""s, { "ToString Test: C string",        []() { return sqf::value("test"s).to_string(); } });
    tester.assert_equals("\"\"\"foo\"\" \"\"bar\"\"\""s, { "ToString Test: std::string",     []() { return sqf::value("\"foo\" \"bar\"").to_string(); } });
    tester.assert_equals("0"s, { "ToString Test: Scalar (int)",    []() { return sqf::value(0).to_string(); } });
    tester.assert_equals("1.2"s, { "ToString Test: Scalar (float)",  []() { return sqf::value(1.2).to_string(); } });
    tester.assert_equals("false"s, { "ToString Test: boolean (false)", []() { return sqf::value(false).to_string(); } });
    tester.assert_equals("true"s, { "ToString Test: boolean (true)",  []() { return sqf::value(true).to_string(); } });
    tester.assert_equals("[]"s, { "ToString Test: Array Empty",     []() { return sqf::value(std::vector<sqf::value>{}).to_string(); } });
    tester.assert_equals(
        "[1,1.2,false,true,\"\"\"foo\"\" \"\"bar\"\"\",nil]"s,
        {
            "ToString Test: Array Filled",
            []() { return sqf::value({1, 1.2, false, true, "\"foo\" \"bar\"", {}}).to_string(); }
        });

    tester.assert_equals(sqf::value(3), { "Addition", []() { return sqf::value(1) + sqf::value(2); } });
    tester.assert_equals(sqf::value(3), { "Addition (double)", []() { return sqf::value(1) + 2.0; } });
    tester.assert_equals(sqf::value(3), { "Addition (float)", []() { return sqf::value(1) + 2.0f; } });
    tester.assert_equals(sqf::value(3), { "Addition (int)", []() { return sqf::value(1) + 2; } });
    tester.assert_equals(sqf::value(3), { "Subtraction", []() { return sqf::value(5) - sqf::value(2); } });
    tester.assert_equals(sqf::value(3), { "Subtraction (double)", []() { return sqf::value(5) - 2.0; } });
    tester.assert_equals(sqf::value(3), { "Subtraction (float)", []() { return sqf::value(5) - 2.0f; } });
    tester.assert_equals(sqf::value(3), { "Subtraction (int)", []() { return sqf::value(5) - 2; } });
    tester.assert_equals(sqf::value(6), { "Multiplication", []() { return sqf::value(2) * sqf::value(3); } });
    tester.assert_equals(sqf::value(6), { "Multiplication (double)", []() { return sqf::value(2) * 3.0; } });
    tester.assert_equals(sqf::value(6), { "Multiplication (float)", []() { return sqf::value(2) * 3.0f; } });
    tester.assert_equals(sqf::value(6), { "Multiplication (int)", []() { return sqf::value(3) * 2; } });
    tester.assert_equals(sqf::value(3), { "Division", []() { return sqf::value(6) / sqf::value(2); } });
    tester.assert_equals(sqf::value(3), { "Division (double)", []() { return sqf::value(6) / 2.0; } });
    tester.assert_equals(sqf::value(3), { "Division (float)", []() { return sqf::value(6) / 2.0f; } });
    tester.assert_equals(sqf::value(3), { "Division (int)", []() { return sqf::value(6) / 2; } });

    tester.assert_true({ "== Test: NIL",             []() { return sqf::value() == sqf::value(); } });
    tester.assert_true({ "== Test: C string",        []() { return sqf::value("test") == "test"s; } });
    tester.assert_true({ "== Test: std::string",     []() { return sqf::value("\"foo\" \"bar\"") == "\"foo\" \"bar\""; } });
    tester.assert_true({ "== Test: Scalar (float)",  []() { return sqf::value(1.2) == 1.2; } });
    // tester.assert_true({ "== Test: boolean (false)", []() { return sqf::value(false) == false; } });
    // tester.assert_true({ "== Test: boolean (true)",  []() { return sqf::value(true) == true; } });
    tester.assert_true({ "== Test: Array Empty",     []() { return sqf::value(std::vector<sqf::value>{}) == std::vector<sqf::value>{}; } });
    tester.assert_true({ "== Test: Array Filled",    []() { return sqf::value({1, 1.2, false, true, "\"foo\" \"bar\"", {}}) == sqf::value({1, 1.2, false, true, "\"foo\" \"bar\"", {}}); } });

    tester.assert_false({ "!= Test: NIL",             []() { return sqf::value() != sqf::value(); } });
    tester.assert_false({ "!= Test: C string",        []() { return sqf::value("test") != "test"s; } });
    tester.assert_false({ "!= Test: std::string",     []() { return sqf::value("\"foo\" \"bar\"") != "\"foo\" \"bar\""; } });
    tester.assert_false({ "!= Test: Scalar (float)",  []() { return sqf::value(1.2) != 1.2; } });
    // tester.assert_false({ "!= Test: boolean (false)", []() { return sqf::value(false) != false; } });
    // tester.assert_false({ "!= Test: boolean (true)",  []() { return sqf::value(true) != true; } });
    tester.assert_false({ "!= Test: Array Empty",     []() { return sqf::value(std::vector<sqf::value>{}) != std::vector<sqf::value>{}; } });
    tester.assert_false({ "!= Test: Array Filled",    []() { return sqf::value({1, 1.2, false, true, "\"foo\" \"bar\"", {}}) != sqf::value({1, 1.2, false, true, "\"foo\" \"bar\"", {}}); } });

    tester.assert_equals(sqf::value({ 1,2,3,4,5 }), { "Parse Test", []() { return sqf::value::parse("[1,2,3,4,5]"); } });
    tester.assert_equals(sqf::value({ 1,2,{ 1,2,3,4,5 },4,5 }), { "Parse Test", []() { return sqf::value::parse("[1,2,[1,2,3,4,5],4,5]"); } });
    tester.assert_equals(sqf::value({ 1, "false", false, "\"foo\"" }), { "Parse Test", []() { return sqf::value::parse("[1,\"false\", false, \"\"\"foo\"\"\"]"); } });
    tester.assert_equals(sqf::value(1), { "Parse Test", []() { return sqf::value::parse("1"); } });
    tester.assert_equals(sqf::value(false), { "Parse Test", []() { return sqf::value::parse("false"); } });
    tester.assert_equals(sqf::value("test"), { "Parse Test", []() { return sqf::value::parse("\"test\""); } });

    tester.assert_equals(sqf::value({ 1,2,3,4,5 }) , { "template<T> value(T t) Constructor with vector<int>",    []() { return sqf::value(std::vector<int>{1,2,3,4,5}); } });
    tester.assert_equals(sqf::value({ 1,2,3,4,5 }) , { "template<T> value(T t) Constructor with array<int>",    []() { return sqf::value(std::array<int, 5>{1,2,3,4,5}); } });
    tester.assert_equals(sqf::value(2) , { "Index Operator GET",    []() { return sqf::value(std::array<int, 5>{1,2,3,4,5})[1]; } });
    tester.assert_equals(sqf::value(2), { "Index Operator SET",    []() { auto val = sqf::value(std::array<int, 5>{0,0,0,0,0}); val[1] = 2; return val[1]; } });

    return tester.all_passed() ? 0 : -1;
}