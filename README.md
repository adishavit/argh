![logo](assets/argh_logo_small.png)

> *Frustration-free command line processing*

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
[![Try it online](https://img.shields.io/badge/try%20it-online-orange.svg)](http://melpon.org/wandbox/permlink/ralxPN49F7cUY2yw)
[![Build Status](https://travis-ci.org/adishavit/argh.svg?branch=master)](https://travis-ci.org/adishavit/argh)

So many different command line processing libraries out there and none of them just work!  
Some bring their whole extended family of related and unrelated external dependencies (*yes, I'm looking at you Boost*).  
Some require quirky syntax and/or very verbose setups that sacrifice simplicity for the generation of a cute usage message and validation. Many come to dominate your `main()` file and yet others do not build on multiple plaforms - for some even their own tests and trivial usage cause crashes on some systems. *Argh!*

If you're writing a highly-sophisticated command line tool, then `Boost.Program_options` and its kind might give you many advanced options. However, if you need to get up and running quickly, effectively and with minimal fuss, give the single header-file `argh` a try.

## TL;DR
It doesn't get much simpler than this:
```cpp
#include <iostream>
#include "argh.h"

int main(int, char* argv[])
{
    argh::parser cmdl(argv);

    if (cmdl[{ "-v", "--verbose" }])
        std::cout << "Verbose, I am.\n";

    return EXIT_SUCCESS;
}
```
## Philosophy

Contrary to many alternatives, `argh` takes a minimalist *laissez-faire* approach, very suitable for fuss-less prototyping with the following rules:

The API is:
 - Minimalistic but expressive:
    - No getters nor binders
    - Just the `[]` and `()` operators
    - Easy iteration (range-`for` too)
 - You don't pay for what you don't use
 - Conversion to typed variables happens (via `std::istream >>`) on the user side *after* the parsing phase
 - No exceptions thrown for failures
 - Liberal BSD license
 - Single header file
 - No non-`std` dependencies

`argh` does **not** care about:

 - How many '`-`' preceded your option
 - Which flags and options you support - that is your responsibility
 - Syntax validation: *any* command line is a valid (*not necessarily unique*) combination of positional *parameters*, *flags* and *options*
 - Automatically producing a usage message

## Tutorial

Create parser:

```cpp
auto cmdl = argh::parser(argc, argv);
```
In fact, you can even drop `argc`. This will also work:  
```cpp
argh::parser cmdl(argv);
```

Positional argument access by (integer) index with `[<size_t>]`:
```cpp
cout << "Exe name is: " << cmdl[0] << '\n';
                               ^^^
assert(cmdl[10000].empty()); // out-of-bound index returns empty string
            ^^^^^
```
Boolean flag argument access by (string) name with `[<std::string>]`:
```cpp
cout << "Verbose mode is " << ( cmdl["verbose"] ? "ON" : "OFF" ) << '\n';
                                    ^^^^^^^^^^^
```
Any dashes are trimmed so are not required.  

Your flag can have several alternatives, just list them with `[{ "<name-1>", "<name-2>", ... }]`:
```cpp
cout << "Verbose mode is " << ( cmdl[{ "-v", "--verbose" }] ? "ON" : "OFF" ) << '\n';
                                    ^^^^^^^^^^^^^^^^^^^^^^^
```
Beyond `bool` and `std::string` access with `[]`, as shown above, we can also access the argument values as an `std::istream`. This is very useful for type conversions.

`std::istream` positional argument access by (integer) index with `(<size_t>)`:
```cpp
std::string my_app_name;
cmdl(0) >> my_app_name; // streaming into a string
    ^^^
cout << "Exe name is: " << my_app_name << '\n';
```
We can also check if a particular positional arg was given or not (this is like using `[<std::string>]` above):
```cpp
if (!cmdl(10))
  cerr << "Must provide at least 10 arguments!" << '\n';
else if (cmdl(11))
  cout << "11th argument  is: " << cmdl[11] << '\n';
```
But we can also set default values for positional arguments. These are passed as the second argument:
```cpp
float scale_factor;
cmdl(2, 1.0f) >> scale_factor;
     ^^^^^^^
```
If the position argument was not given or the streaming conversion failed, the default value will be used.  

Similarly, parameters can be accessed by name(s) (i.e. by string or list of string literals) with:  
`(<std::string> [, <default value>])` or   `({ "<name-1>", "<name-2>", ... } [, <default value>])`:  
```cpp
float scale_factor;
cmdl("scale", 1.0f) >> scale_factor; // Use 1.0f as default value
     ^^^^^^^^^^^^^

float threshold;
if (!(cmdl({ "-t", "--threshold"}) >> threshold)) // Check for missing param and/or bad (inconvertible) param value
  cerr << "Must provide a valid threshold value! Got '" << cmdl("threshold").str() << "'" << endl;
else                                                                        ^^^^^^
  cout << "Threshold set to: " << threshold << '\n';
```
As shown above, use `std::istream::str()` to get the param value as a `std:string` or just stream the value into a variable of a suitable type. Standard stream state indicates failure, including when the argument was not given.  
When using multiple names, the first value found will be returned.

Positional arguments can be iterated upon directly using *range-for*:
```cpp
cout << "Positional args:\n";
for (auto& pos_arg : cmdl)
  cout << '\t' << pos_arg << '\n';
```
Similarly, `cmdl.size()` will return the count of *positional* arguments. 

Positional arguments, flags *and* parameters are accessible as "ranges":
```cpp
cout << "Positional args:\n";
for (auto& pos_arg : cmdl.pos_args())
  cout << '\t' << pos_arg << '\n';

cout << "\nFlags:\n";
for (auto& flag : cmdl.flags())
  cout << '\t' << flag << '\n';

cout << "\nParameters:\n";
for (auto& param : cmdl.params())
  cout << '\t' << param.first << " : " << param.second << '\n';
```


By default, options are assumed to be boolean flags. 
When this is not what you want, there are several ways to specify when an option is a parameter with an associated value.  

Specify **`PREFER_PARAM_FOR_UNREG_OPTION`** mode to interpret *any* `<option> <non-option>` as `<parameter-name> <parameter-value>`:
```cpp
using namespace argh;
auto cmdl = parser(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
                               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
cout << cmdl("--threshold").str() << '\n';
```
Pre-register an expected parameter name with `add_param()` (before calling `parse()`):
```cpp
argh::parser cmdl;
cmdl.add_param("threshold"); // pre-register "threshold" as a param: name + value
cmdl.parse(argc, argv);
cout << cmdl("threshold").str() << '\n';
```
You can also *batch* pre-register multiple options as parameters with `add_params({ ... })`:
```cpp
argh::parser cmdl;
cmdl.add_params({ "-t", "--threshold", "-s", "--scale" }); // batch pre-register multiple params: name + value
cmdl.parse(argc, argv);
cout << cmdl("threshold").str() << '\n';
```
Since pre-registration has to be done *before* parsing, we might as well just use the ctor:
```cpp
argh::parser cmdl({ "-t", "--threshold", "-s", "--scale" }); // batch pre-register multiple params: name + value
cmdl.parse(argc, argv);
cout << cmdl("threshold").str() << '\n';
```
Use a `=` (with no spaces around it) within the option when *calling* the app:
```cpp
>> my_app --threshold=42
42
```

### Tips
- By default, arguments of the form `--<name>=<value>` (with no spaces, one or more dashes), e.g. `--answer=42`, will be parsed as `<parameter-name> <parameter-value>`.
To disable this specify the **`NO_SPLIT_ON_EQUALSIGN`** mode.
- Specifying the **`SINGLE_DASH_IS_MULTIFLAG`** mode will split a single-hyphen argument into multiple single-character flags (as is common in various POSIX tools).
- When using **`SINGLE_DASH_IS_MULTIFLAG`**, you can still pre-register the last character as a param with the value, such that if we pre-register `f` as a param, `>> myapp -xvf 42` will be parsed with two boolean flags `x` and `v` and a one param `f`=`42`.
- When parsing parameter values as strings that may contain spaces (e.g. `--config="C:\Folder\With Space\Config.ini"`), prefer using `.str()` instead of `>>` to avoid the default automatic whitespace input stream tokenization:  
`cout << cmdl({ "-c", "--config" }).str()`.

## Terminology
Any command line is composed of **2** types of *Args*:

1. ***Positional Args***:  
    Free standing, in-order, values  
    e.g. `config.json`  
2. ***Options***:  
    Args beginning with `-` (and that are not negative numbers).  
    We identify ***2*** kinds of *Options*:  
    1. ***Flags***:  
       Boolean options =>  (appear ? true : false)  
       e.g. `-v`, `--verbose`  
    2. ***Parameters***:  
       A named value followed by a *non*-option value  
       e.g. `--gamma 2.2`  

Thus, any command line can always be broken into some combination of *(1) positional args* *(2) flags* and *(3) parameters*.  

## API Summary
### Parsing
Parse the command line using either
- The `parse()` method: `parser::parse([argc,] argv [, mode])`; or
- The shorter form using the ctor directly:
  `argh::parser([argc,] argv [, mode]);`
- The shortest form does not even require `argc`, so in default `mode` just use:   
  `parser(argv);`

### Special Parsing Modes
Extra flexibility can be added be specifying parsing modes:
- **`NO_SPLIT_ON_EQUALSIGN`**:
   By default, an option of the form `--pi=22/7` will be parsed as a *parameter* `pi` with an associated value `"22/7"`.
   By setting this mode, it will be not be broken at the `=`.
- **`PREFER_FLAG_FOR_UNREG_OPTION`**:
  Split `<option> <non-option>` into `<flag>` and `<pos_arg>`.
  e.g. `myapp -v config.json` will have `v` as a lit flag and `config.json` as a positional arg.
  *This is the default mode.*
- **`PREFER_PARAM_FOR_UNREG_OPTION`**:
  Interpret `<option> <non-option>` as `<parameter-name> <parameter-value>`.
  e.g. `myapp --gamma 2.2` will have `gamma` as a parameter with the value "2.2".
- **`SINGLE_DASH_IS_MULTIFLAG`**:
  Splits an option with a *single* dash into separate boolean flags, one for each letter.
  e.g. in this mode, `-xvf` will be parsed as 3 separate flags: `x`, `v`, `f`.

### Argument Access
- Use *bracket operators* to access *flags* and *positional* args:
    - Use `operator[index]` to access *position* arg strings by *index*:
        - e.g. `assert(cmdl[0] == argv[0])`, the app name.
    - Use `operator[string]` to access boolean *flags* by *name*:
        - e.g. `if (cmdl["v"]) make_verbose();`
    - Use `operator[{...}]` to access boolean *flags* by *multiple names*:
        - e.g. `if (cmdl[{ "v", "verbose" }]) make_verbose();`
  
- Use the *parenthesis operators* to get an `std::istream` to stream values from *parameters* and *positional* args:
    - Use `operator(index)` to access position arg `istream` by index:
        - e.g. `cmdl(0) >> my_app_name`.
    - Use `operator(string)` to access *parameter* values by *name*:
        - e.g. `cmdl("scale") >> scale_factor;`
    - Use `operator({...})` to access *parameter* values by *multiple names*:
        - e.g. `cmdl({ "-s", "--scale" }) >> scale_factor;`        
    - Use `operator(index, <default>)` and `operator(string/{list}, <default>)` to stream a default value if the arg did not appear on the command line:
        - e.g. `cmdl("scale", 1.0f) >> scale_factor;`

The streaming happens at the user's side, so conversion failure can be checked there:
e.g

```cpp
if (!(cmdl("scale") >> scale_factor))
  cerr << "Must provide valid scale factor!" << '\n';
```

Use the `.str()` method to get the parameter value as a string: e.g. `cmdl("name").str();`

### More Methods

- Use `parser::add_param()`, `parser::add_params()` or the `parser({...})` constructor to *optionally* pre-register a parameter name when in `PREFER_FLAG_FOR_UNREG_OPTION` mode.
- Use `parser`, `parser::pos_args()`, `parser::flags()` and `parser::params()` to access and iterate over the Arg containers directly.

## Finding Argh!

* copy `argh.h` somewhere into your projects directories
* **or** include the repository as a *submodule*
* **or** use *CMake*!

#### Finding Argh! - CMake

The provided `CMakeLists.txt` generates targets for tests, a demo application and an install target to install `argh` system-wide and make it known to CMake.  *You can control generation of* test *and* example *targets using the options `BUILD_TESTS` and `BUILD_EXAMPLES`. Only `argh` alongside its license and readme will be installed - not tests and demo!*


Add `argh` to your CMake-project by using
```cmake
find_package(argh)
```
The package exports `argh` *INTERFACE* library target and `argh_INCLUDE_DIR` variable. Make `argh.h` known to your compiler by using one of the following methods; both will make the location of `argh.h` known to the compiler, not link in a precompiled library - even when using `target_link_libraries()`.
```cmake
target_include_directories(${MY_TARGET_NAME} PRIVATE "${argh_INCLUDE_DIR}")
#OR
target_link_libraries(${MY_TARGET_NAME} argh)

```

## Buck

The [Buck](https://buckbuild.com/) build system is also supported. 

Run the example: 

```bash=
buck run :example
```

Run the tests: 

```bash=
buck run :tests
buck run test_package
```

If you take `argh` as a submodule, then the visible target is `//:argh`. 

## Colophon

I ❤ your feedback. If you found Argh! useful - do Tweet about it to let [me](https://twitter.com/AdiShavit) know. If you found it lacking, please post an [issue](https://github.com/adishavit/argh/issues).
