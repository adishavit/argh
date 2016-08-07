# WORK in PROGRESS   
# Argh! The Minimalist Argument Handler
> *Argh! Why does simple command line processing need to be such a pain!*

*Argh!* So many different command line processing libraries out there and none of them just work! Some bring their whole extended family of related and unrelated external dependencies, some require quirky syntax and/or very verbose setups that sacrifice simplicity for the generation of a cute usage message and validation. Others do not build on multiple plaforms - even their own tests and trivial usage cause a crash on some systems. *Argh!*

If you're writing a product quality highly-sophisticated command line tool, then `Boost.Program_options` and its kin should be your go-to tools (you may already even have Boost as a dependency anyway).  
However, if you need just a few options with minimal fuss give `argh` a try.
 
##TL;DR
```cpp
#include "argh.h"

int main(int argc, char* argv[])
{
    argh::parser cmdline;
    cmdline.parse(argc, argv);

    if (cmdline["-v"])
        std::cout << "Verbose, I am." << std::endl;

    return EXIT_SUCCESS;
}
```

##Philosophy
Contrary to many alternatives, `argh` takes a *laissez-faire* approach, very suitable for fussless prototyping with the following rules:
 
 1. Any non-number *arg* beginning with a `-` is an *option*;
 2. *Otherwise* it is a free, positional, *parameter*.
 3. Unless *pre*-registered, all *options* are (boolean) *flags*;
 4. A *pre*-registered *option* gets the following *free parameter* as its value. 
    If the following arg is *not* a *free parameter*, then the *option* is deemed a (valueless) boolean *flag*.

`argh` does not care about:
 - how many `-` preceded your option;
 - which flags and options you support - that is your responsibility;
 - syntax validation: *any* command line is a valid combination of positional *parameters*, *flags* and *options*;
 - up-front type validation: type conversions happen when trying to access an *option* value;
 - automatically producing a usage message.


 The API is designed to be:
 - non-throwing: 
 - non-failing
 - convenient
 - You don't pay for what you don't use
 - post-binding: bind a typed variable to an input option, only when and where you need it.