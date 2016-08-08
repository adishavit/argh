#include <iostream>

#include "argh.h"

/*
int main(int argc, char* argv[])
{
    argh::parser cmdline;
    cmdline.parse(argc, argv);

    if (cmdline["-v"])
        std::cout << "Verbose, I am." << std::endl;

    return EXIT_SUCCESS;
}
*/


using namespace std;

int main(int argc, char* argv[])
{
    argh::parser cmdl;
    cmdl.add_param("-f");
    cmdl.parse(argc, argv);

    if (cmdl["-f"])
        cout << "Got -f" << endl;

    cout << cmdl["f"] << endl;

    int fval = -1;
    if (cmdl("f") >> fval)
        cout << fval << endl;

    cmdl("xxxx") >> (fval = -999);
    cout << fval << endl;

    cout << "Flags:\n";
    for (auto it = cmdl.flags_begin(); it != cmdl.flags_end(); ++it)
        cout << "\t" << *it << endl;

    cout << "Options:\n";
    for (auto it = cmdl.params_begin(); it != cmdl.params_end(); ++it)
        cout << "\t" << it->first << " = " << it->second << endl;

    int k = -1;
    if (cmdl(1) >> k)
        cout << "First pos arg is " << k << endl;
    else
        cout << "Could not convert first arg :-(  : '" << cmdl[1] << "', k == " << k << endl;

    cout << "Free args:\n";
    for (auto const& pos_arg : cmdl)
        cout << "\t" << pos_arg << endl;

    cout << "Free args 2:\n";
    for (int i=0; i<cmdl.size(); ++i)
        cout << "\t" << cmdl(i).str() << endl;

    return EXIT_SUCCESS;
}


