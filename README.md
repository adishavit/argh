# *"Argh! A minimalist argument handler!"*
> *Argh! Why does simple command line processing need to be such a pain!*

*Argh!* So many different command line processing libraries out there and none of them just work! Some bring their whole extended family of related and unrelated external dependencies, some require quirky syntax and/or very verbose setups that sacrifice simplicity for the generation of a cute usage message and validation. Others do not build on multiple plaforms - for some even their own tests and trivial usage cause crashes on some systems. *Argh!*

If you're writing a product quality highly-sophisticated command line tool, then `Boost.Program_options` and its kin should be your go-to tools (you may already even have Boost as a dependency anyway).  
However, if you need just a few options with minimal fuss give the single header file `argh` a try.
 
##TL;DR
```cpp
#include "argh.h"

int main(int argc, const char* argv[])
{
    argh::parser cmdl
    cmdl.parse(argc, argv);

    if (cmdl["-v"])
        std::cout << "Verbose, I am." << std::endl;

    return EXIT_SUCCESS;
}
```
##Terminology
Any command line is composed of **2** types of *Args*:
1. ***Positional Args***:
    Free standing, in-order, values
    e.g. `config.json`
3. ***Options***: 
    Args beginning with `-` (and that are not negative numbers). 
    We identify ***2*** kinds of *Options*:
    1. ***Flags***: 
       Boolean options =>  (appear ? true : false)
       e.g. `-v`, `--verbose`
    2. ***Parameters***: 
       A named value followed by a *non*-option value
       e.g. `--gamma 2.2`

Thus, any command line can be broken into *(1) positional args* *(2) flags* and *(3) parameters*.

## Brief API Summary
- Use `parser::parse(argc, argv[], mode)` to parse the command line; 
- Parsing modes:
    - `parser::PREFER_FLAG_FOR_UNREG_OPTION`: Split `<option> <non-option>` into `<flag>` and `<pos_arg>`. 
    e.g. `myapp -v config.json` will have `v` as a lit flag and `config.json` as a positional arg.
    *This is the default mode.*
    - `parser::PREFER_PARAM_FOR_UNREG_OPTION`: Interpret `<option> <non-option>` as `<parameter-name> <parameter-value>`. 
    e.g. `myapp --gamma 2.2` will have `gamma` as a parameter with the value "2.2".    
- Use `parser::add_param()` to *optionally* pre-register a parameter name when in `PREFER_FLAG_FOR_UNREG_OPTION` mode.
- Use `parser::pos_args()`, `parser::flags()` and `parser::params()` to access and iterate over the Arg containers directly.
- Use *bracket operators* to access flags and positional args: 
    - Use `operator[index]` to access position arg strings: e.g. `cmdl[0] == argv[0]`, the app name.
    - Use `operator[string]` to access boolean flags by name: e.g. `if (cmdl["v"]) make_verbose();`
- Use the *function call operators* to get an `std::istream` to stream values from paramters and positional args:
    - Use `operator(index)` to access position arg `istream`: e.g. `cmdl(0) >> my_val`.
    - Use `operator(string)` to access parameter values by name: e.g. `cmdl("scale") >> scale_factor;`
    - Use `operator(index, <default>)` and `operator(string, <default>)` to stream a default value if the arg did not appear on the command line: `cmdl("scale", 1.0f) >> scale_factor;`
    - The streaming happens at the user's side, so conversion failure can be checked there: 
    e.g `if (!(cmdl("scale") >> scale_factor)) cerr << "Must provide valid scale factor! << endl;`

##Philosophy
Contrary to many alternatives, `argh` takes a minimalist *laissez-faire* approach, very suitable for fuss-less prototyping with the following rules:

The API is:
 - Minimalistic but expressive: 
    - No getters nor binders
    - Just bracket`[]` and function`()` operators. 
    - Easy iteration (range-`for` too).
 - You don't pay for what you don't use;
 - Conversion to typed variables happens (via `std::istream >>`) on the user side *after* the parsing phase.

`argh` does not care about:
 - How many `-` preceded your option;
 - Which flags and options you support - that is your responsibility;
 - Syntax validation: *any* command line is a valid combination of positional *parameters*, *flags* and *options*;
 - Automatically producing a usage message.

 