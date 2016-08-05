#include <iostream>

#include "argh.h"

using namespace std;

int main(int argc, char* argv[])
{
    argh::parser cmdline;
    cmdline.add_option("-f");
    cmdline.parse(argc, argv);

    if (cmdline.flag("-f"))
        cout << "Got -f" << endl;

    cout << cmdline.get<std::string>("f").value_or("Nope") << endl;
    cout << cmdline["f"] << endl;

    int fval;
    if (cmdline.get("f", fval))
        cout << fval << endl;

    if (cmdline.get("xxxx", fval, -999))
        cout << fval << endl;

    cout << "Flags:\n";
    for (auto it = cmdline.flags_cbegin(); it != cmdline.flags_cend(); ++it)
        cout << "\t" << *it << endl;

    cout << "Options:\n";
    for (auto it = cmdline.options_cbegin(); it != cmdline.options_cend(); ++it)
        cout << "\t" << it->first << " = " << it->second << endl;


    cout << "Free params:\n";
    for (auto it = cmdline.params_cbegin(); it != cmdline.params_cend(); ++it)
        cout << "\t" << *it << endl;

    return EXIT_SUCCESS;
}