# How to use

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
std::cout << arr.as<std::vector<value>>()[4] << std::endl; /* `5` */

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

# How to install

Just drop the `value.h` file inside of your project and you are golden!

As alternative, you may also add this as submodule and setup your environment like a pro.
