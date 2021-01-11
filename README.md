# How to install

Just drop the corresponding files inside of your project and you are golden!

As alternative, you may also add this as submodule and setup your environment like a pro.

# Components
## Methodhost

## SQF-Value
### How to use

Using *sqf-value* is rather straight forward.
You just add the `#include "value.h"` to the top of your C++ file and can start going!

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