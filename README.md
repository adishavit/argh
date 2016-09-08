# *"Argh! A minimalist argument handler!"*
> *Argh! Why does simple command line processing need to be such a pain!*

*Argh!* So many different command line processing libraries out there and none of them just work! Some bring their whole extended family of related and unrelated external dependencies, some require quirky syntax and/or very verbose setups that sacrifice simplicity for the generation of a cute usage message and validation. Others do not build on multiple plaforms - for some even their own tests and trivial usage cause crashes on some systems. *Argh!*

If you're writing a highly-sophisticated command line tool, then `Boost.Program_options` and its kin should be your go-to tools (you may already even have Boost as a dependency anyway).
However, if you need just a few options with minimal fuss give the single header file `argh` a try.

## TL;DR
It doesn't get much simpler than this:
```cpp
#include "argh.h"

int main(int argc, const char* argv[])
{
    argh::parser cmdl(argc, argv);

    if (cmdl["-v"])
        std::cout << "Verbose, I am." << std::endl;

    return EXIT_SUCCESS;
}
```
## Philosophy
Contrary to many alternatives, `argh` takes a minimalist *laissez-faire* approach, very suitable for fuss-less prototyping with the following rules:

The API is:
 - Minimalistic but expressive:
    - No getters nor binders
    - Just the `[]` and `()` operators.
    - Easy iteration (range-`for` too).
 - You don't pay for what you don't use;
 - Conversion to typed variables happens (via `std::istream >>`) on the user side *after* the parsing phase;
 - No exceptions thrown for failures.

`argh` does not care about:
 - How many `-` preceded your option;
 - Which flags and options you support - that is your responsibility;
 - Syntax validation: *any* command line is a valid combination of positional *parameters*, *flags* and *options*;
 - Automatically producing a usage message.

## Tutorial
Create parser:
```cpp
auto cmdl = argh::parser(argc, argv);
```
Positional argument access by (integer) index with `[<size_t>]`:
```cpp
cout << "Exe name is: " << cmdl[0] << endl;
                               ^^^
assert(cmdl[10000].empty()); // out-of-bound index returns empty string
            ^^^^^
```
Boolean flag argument access by (string) name with `[<std::string>]`:
```cpp
cout << "Verbose mode is " << ( cmdl["verbose"] ? "ON" : "OFF" ) << endl;
                                    ^^^^^^^^^^^
```
Beyond `bool` and `std::string` access with `[]`, as shown above, we can also access the argument values as an `std::istream`. This is very useful for type conversions.

`std::istream` positional argument access by (integer) index with `(<size_t>)`:
```cpp
std::string my_app_name;
cmdl(0) >> my_app_name; // streaming into a string
    ^^^
cout << "Exe name is: " << my_app_name << endl;
```
We can also check if a particular positional arg was given or not (this is like using `[<std::string>]` above):
```cpp
if (!cmdl(10))
  cerr << "Must provide at least 10 arguments! << endl;
else if (cmdl(11))
  cout << "11th argument  is: " << cmdl[11] << endl;
```
But we can also set default values for positional arguments. These are passed as the second argument:
```cpp
float scale_facor;
cmdl(2, 1.0f) >> scale_factor;
     ^^^^^^^
```
If the position argument was not given or the streaming conversion failed, the default value will be used.  
Similarly, parameters can be accessed by (string) name with `(<std::string> [, <default value>])`:
```cpp
float scale_facor;
cmdl("scale", 1.0f) >> scale_factor; // Use 1.0f as default value
     ^^^^^^^^^^^^^

float threshold;
if (!(cmdl("threshold") >> theshold)) // Check for missing param and/or bad (inconvertible) param value
  cerr << "Must provide a valid threshold value! Got '" << cmdl("threshold").str() << "'" << endl;
else                                                                        ^^^^^^
  cout << "Threshold set to: " << threshold << endl;
```
As shown above, use `std::istream::str()` to get the param value as a `std:string` or just stream the value into a variable of a suitable type. Standard stream state indicates failure, including when the argument was not given.

Positional arguments, flags and parameters are accessible as ranges:
```cpp
cout << "Positional args:\n";
for (auto& pos_arg : cmdl.pos_args())
  cout << '\t' << pos_arg << endl;

cout << "\nFlags:\n";
for (auto& flag : cmdl.flags())
  cout << '\t' << flag << endl;

cout << "\nParameters:\n";
for (auto& param : cmdl.params())
  cout << '\t' << param.first << " : " << param.second << endl;
```

By default options are assumed to be boolean flags. 
When this is not what you want, there are several ways to specify when an option is a parameter with an associated value.  

Specify **`PREFER_PARAM_FOR_UNREG_OPTION`** mode to interpret *any* `<option> <non-option>` as `<parameter-name> <parameter-value>`:
```cpp
using namespace argh;
auto cmdl = parser(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
                               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
std::cout << cmdl("threshold").str() << std::endl;
```
Pre-register an expected parameter name:
```cpp
argh::parser cmdl;
cmdl.add_param("threshold"); // pre-register "threshold" as a param: name + value
cmdl.parse(argc, argv);
std::cout << cmdl("threshold").str() << std::endl;
```
Use a `=` (with no spaces around it) within the option when *calling* the app:
```cpp
>> my_app --threshold=42
42
```

### Tips
- By default, arguments of the form `--<name>=<value>` (with no spaces), e.g. `--answer=42`, will be parsed as `<parameter-name> <parameter-value>`.
To disable this specify the **`NO_SPLIT_ON_EQUALSIGN`** mode.
- Specifying the **`SINGLE_DASH_IS_MULTIFLAG`** mode will split a single-hyphen argument into multiple single-character flags (as is common in various POSIX tools).

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
- The `parse()` method: `parser::parse(argc, argv, mode)`; or
- The shorter form using the ctor directly:
  `argh::parser cmdl(argc, argv, mode);`

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

- Use the *parenthesis operators* to get an `std::istream` to stream values from *paramters* and *positional* args:
    - Use `operator(index)` to access position arg `istream` by index:
        - e.g. `cmdl(0) >> my_app_name`.
    - Use `operator(string)` to access *parameter* values by *name*:
        - e.g. `cmdl("scale") >> scale_factor;`
    - Use `operator(index, <default>)` and `operator(string, <default>)` to stream a default value if the arg did not appear on the command line:
        - e.g. `cmdl("scale", 1.0f) >> scale_factor;`

The streaming happens at the user's side, so conversion failure can be checked there:
e.g

```cpp
if (!(cmdl("scale") >> scale_factor))
  cerr << "Must provide valid scale factor! << endl;
```

Use the `.str()` method to get the parameter value as a string: e.g. `cmdl("name").str();`

### More Methods
- Use `parser::add_param()` to *optionally* pre-register a parameter name when in `PREFER_FLAG_FOR_UNREG_OPTION` mode.
- Use `parser::pos_args()`, `parser::flags()` and `parser::params()` to access and iterate over the Arg containers directly.

