#pragma once

#include "method.hpp"


namespace sqf
{
    class methodhost
    {
    public:
        static constexpr int exec_ok = 0;
        static constexpr int exec_err = -1;
        static constexpr int exec_more = 1;
        static constexpr const char* msg_unknown_method = "Method passed is not known to extension.";
    private:
        class long_result
        {
            std::string value;
            size_t m_index;
            bool m_is_error;
        public:
            size_t key;
            long_result(bool is_error, size_t key, std::string str) : value(str), m_index(0), key(key)
            {

            }
            void next(char* output, size_t size)
            {
                if (size == 0) { return; }

                // move size one back for '\0'
                size--;

                // get start and end of writable value
                auto len = value.length();
                auto start = m_index;
                auto end = start + size > len ? len : start + size;

                // copy value to output
                strncpy(output, value.data() + start, end - start);

                // Set end to '\0'
                output[end - start] = '\0';

                // set m_index to end
                m_index = end;
            }
            bool is_done() const { return value.length() <= m_index; }
            bool is_error() const { return m_is_error; }
        };

        std::unordered_map<std::string, std::vector<method>> m_map;
        std::vector<long_result> m_long_results;
        size_t m_long_result_keys;

        methodhost(std::unordered_map<std::string, std::vector<method>> map) : m_long_result_keys(0), m_map(map)
        {
        }

        static void copy_string(std::string s, char* output, size_t output_size)
        {
            sqf::value val = s;
            auto str = val.to_string();
            strncpy(output, str.data(), str.length());
            output[str.length()] = '\0';
        }
    public:
        static methodhost& instance();

        int execute(char* output, int outputSize, const char* in_function, const char** argv, int argc)
        {
            // Put in_function into fancy string
            std::string function(in_function);

            // Read in values
            std::vector<sqf::value> values;
            for (size_t i = 0; i < argc; i++)
            {
                values.push_back(sqf::value::parse(argv[i]));
            }
            
            // Check if long-result continuation was requested
            if (function == "?")
            {
                if (values.size() != 1)
                {
                    copy_string("Argument count mismatch! Expected 1.", output, outputSize);
                    return exec_err;
                }

                size_t key = (size_t)(float(values[0]));
                auto lr = std::find_if(
                    m_long_results.begin(),
                    m_long_results.end(),
                    [key](long_result& res) -> bool { return res.key == key; });

                if (lr == m_long_results.end())
                {
                    copy_string("Long Result key unknown or expired.", output, outputSize);
                    return exec_err;
                }
                lr->next(output, outputSize);
                if (lr->is_done())
                {
                    if (lr->is_error())
                    {
                        m_long_results.erase(lr);
                        return exec_err;
                    }
                    else
                    {
                        m_long_results.erase(lr);
                        return exec_ok;
                    }
                }
                else
                {
                    return exec_more;
                }
            }
            else
            {
                // Check if matching method via name can be found
                auto method_name_find_res = m_map.find(function);
                if (method_name_find_res == m_map.end())
                {
                    copy_string("No matching method found.", output, outputSize);
                    return exec_err;
                }

                // Check if method matches with args
                auto method_args_find_res = std::find_if(
                    method_name_find_res->second.begin(),
                    method_name_find_res->second.end(),
                    [values](method& m) -> bool { return m.can_call(values); }
                );
                if (method_args_find_res == method_name_find_res->second.end())
                {
                    copy_string("No matching overload found.", output, outputSize);
                    return exec_err;
                }

                // Execute actual method
                auto retval = method_args_find_res->call_generic(values);

                std::string result = (retval.is_ok() ? retval.get_ok() : retval.get_err()).to_string();

                if (result.length() + 1 > outputSize)
                {
                    auto key = ++m_long_result_keys;
                    m_long_results.emplace_back(retval.is_err(), key, result);
                    auto key_string = sqf::value((float)key).to_string();
                    strncpy(output, key_string.data(), key_string.length());
                    output[key_string.length()] = '\0';
                    return exec_more;
                }
                else
                {
                    strncpy(output, result.data(), result.length());
                    output[result.length()] = '\0';
                    return retval.is_err() ? exec_err : exec_ok;
                }
            }
        }
    };
}