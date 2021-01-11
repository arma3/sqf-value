#include "value.hpp"
#include "tester.hpp"

#undef assert

using namespace std::string_literals;


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

    tester.assert_equals(sqf::value({ 1, 2, 3, 4, 5 }), { "\"...\"_sqf",    []() { using namespace sqf; return "[1,2,3,4,5]"_sqf; } });
    tester.assert_equals(sqf::value(), { "\"\"_sqf",    []() { using namespace sqf; return ""_sqf; } });

    tester.assert_equals(true,                      { "sqf::is<float>(sqf::value(1))",                  []() { return sqf::is<float>(sqf::value(1)); } });
    tester.assert_equals(true,                      { "sqf::is<bool>(sqf::value(true))",                []() { return sqf::is<bool>(sqf::value(true)); } });
    tester.assert_equals(true,                      { "sqf::is<void>(sqf::value())",                    []() { return sqf::is<void>(sqf::value()); } });
    tester.assert_equals(true,                      { "sqf::is<std::vector<sqf::value>>(sqf::value(std::vector<sqf::value>()))", []() { return sqf::is<std::vector<sqf::value>>(sqf::value(std::vector<sqf::value>())); } });
    tester.assert_equals(false,                     { "sqf::is<float>(sqf::value())",                   []() { return sqf::is<float>(sqf::value()); } });
    tester.assert_equals(false,                     { "sqf::is<bool>(sqf::value())",                    []() { return sqf::is<bool>(sqf::value()); } });
    tester.assert_equals(false,                     { "sqf::is<void>(sqf::value(1))",                   []() { return sqf::is<void>(sqf::value(1)); } });
    tester.assert_equals(false,                     { "sqf::is<std::vector<sqf::value>>(sqf::value())", []() { return sqf::is<std::vector<sqf::value>>(sqf::value()); } });
    tester.assert_equals(1,                         { "sqf::get<float>(sqf::value(1))",                 []() { return sqf::get<float>(sqf::value(1)); } });
    tester.assert_equals(true,                      { "sqf::get<bool>(sqf::value(true))",               []() { return sqf::get<bool>(sqf::value(true)); } });
    tester.assert_equals(std::vector<sqf::value>(), { "sqf::get<std::vector<sqf::value>>(sqf::value(std::vector<sqf::value>()))",    []() { return sqf::get<std::vector<sqf::value>>(sqf::value(std::vector<sqf::value>())); } });

    return tester.all_passed() ? 0 : -1;
}