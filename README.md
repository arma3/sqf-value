# How to install

Just drop the corresponding files inside of your project and you are golden!

As alternative, you may also add this as submodule and setup your environment like a pro.

# Components
## Methodhost
For using *methodhost*, you need to add `#include "methodhost.hpp"` to the top of your C++ file and
add the following method to it:
```cpp
sqf::methodhost& sqf::methodhost::instance()
{
    using namespace std::string_literals;
    static sqf::methodhost h({
        });
    return h;
}
```
Adding methods then can be done, by simply adding it to the initializer list:
```cpp
sqf::methodhost h({
    { "my_fancy_method", { sqf::method::create(
        [](
            // required SCALAR value at index 0
            float req_scalar,
            // required STRING value at index 1
            std::string string,
            // required BOOLEAN value at index 2
            bool boolean,
            // optional ARRAY value at index 3
            std::optional<std::vector<sqf::value>> opt_array)
        // note that sqf::methodhost::ret<Tok, Terr> is for returning either
        // a good result or an error result. You also may return any other supported type
        // instead
        -> sqf::methodhost::ret<float, std::string> {
            ...
            // To return a value, you can either do one of the following:
            return { 1.0, {} }; // ok
            return { {}, "something broke" }; // error
            return 1.0; // ok - requires that the error and ok types are different
            return "something broke"; // err - requires that the error and ok types are different
            return sqf::methodhost::ret<float, std::string>::ok(1.0); // ok
            return sqf::methodhost::ret<float, std::string>::err("something broke"); // error
        }
    ), ... } },
};
```
and the methodhost will mangle the functions properly.
Note that `std::optional<...>` can be used for optional parameters and that overloading works only
on the parameters.


To then use the methodhost, you use the following code:
```cpp
__declspec (dllexport) int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc)
{
    auto res = sqf::methodhost::instance().execute(output, outputSize, function, argv, argc);
    return res;
}
```

on the SQF side, scripts can call the extension using:
```sqf
params ["_method", "_args"];
private _result = "";
private _longResult = nil;
// do a call for an extra variable scope
0 call {
    ("extFileIO" callExtension [_method, _args]) params ["_resultData", "_returnCode", "_errorCode"];
    if (_errorCode != 0) then { throw _errorCode; };
    switch _returnCode do {
        case -1: { _result = (parseSimpleArray format["[%1]", _resultData]) select 0; throw _result; };
        case 0: { _result = (parseSimpleArray format["[%1]", _resultData]) select 0; };
        case 1: { _longResult = (parseSimpleArray format["[%1]", _resultData]) select 0; };
    };
};
// while in long result, keep polling
while { !isNil "_longResult" } do
{
    ("extFileIO" callExtension ["?", _longResult]) params ["_resultData", "_returnCode", "_errorCode"];
    if (_errorCode != 0) then { throw _errorCode; };
    switch _returnCode do {
        case -1: { _result = (parseSimpleArray format["[%1]", _result + _resultData]) select 0; throw _result; };
        case 0: { _result = (parseSimpleArray format["[%1]", _result + _resultData]) select 0; _longResult = nil; };
        case 1: { _result = _result + _resultData; };
    };
};
_result
```

## SQF-Value
Using *sqf-value* is rather straight forward.
You just add the `#include "value.hpp"` to the top of your C++ file and can start going!

```cpp
// Creating a string
sqf::value s = "Happyface: \":)\"";
// Gives you
std::cout << s.to_string() << std::endl; /* `"Happyface: "":)"""` */

// Parsing an array?
auto arr = sqf::value::parse("[1, 2, 3, 4, 5]");
// No problem!
std::cout << float(arr[4]) << std::endl; /* `5` */

// Have numerical stuff to solve?
sqf::value val = 12.5;
// just do it!
std::cout << (val + 7.5) << std::endl; /* `20` */
```

as you can see, *sqf-value* got it all and is really simple to use and provides you with the magic horsepower you need to run your extensions as you never did before :)

Ohh and ... by the way ... it also supports nested equality checks:
```cpp
sqf::value val({ 1,2,{ 1,2,3,4,5 },4,5 });
sqf::value other = sqf::value::parse("[1,2,[1,2,3,4,5],4,5]");

std::cout << (val == other) << std::endl; /* `true` */
```
so no need to worry about that either :stuck_out_tongue:

And in the case that you would like to parse raw strings, you can do the following:
```cpp
using namespace sqf;
sqf::value val = "['this is my fancy array', 1, true, nil]"_sqf;
```
If you want to check if something is a certain type, you can do one of the following:
```cpp
sqf::value val = ...;
// check if value is SCALAR
sqf::is<float>(val);
val.is_scalar();
// check if value is STRING
sqf::is<std::string>(val);
val.is_string();
// check if value is BOOLEAN
sqf::is<bool>(val);
val.is_boolean();
// check if value is ARRAY
sqf::is<std::vector<sqf::value>>(val);
val.is_array();
// check if value is NIL
sqf::is<void>(val);
val.is_nil();
```
and for getting a certain type, the following methods are available:
```cpp
sqf::value val = ...;
// get value as SCALAR
sqf::get<float>(val);
val.as_scalar();
// get value as STRING
sqf::get<std::string>(val);
val.as_string();
// get value as BOOLEAN
sqf::get<bool>(val);
val.as_boolean();
// get value as ARRAY
sqf::get<std::vector<sqf::value>>(val);
val.as_array();
```