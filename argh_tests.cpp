#include "argh.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace argh;

TEST_CASE("Test empty cmdl") 
{
    parser cmdl;
    cmdl.parse(0, nullptr);
    CHECK(0 == cmdl.size());
    CHECK(cmdl[0].empty());
    CHECK(cmdl(0).str().empty());
    CHECK(cmdl[10].empty());
    CHECK(cmdl(10).str().empty());
    CHECK(!cmdl["xxx"]);
    CHECK(cmdl("xxx").str().empty());
}

TEST_CASE("Test positional access")
{
    parser cmdl; 
    char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv);
    CHECK(5 == cmdl.size());
    for (auto parg : cmdl)
        CHECK(!parg.empty());
    
    CHECK(cmdl[0] == "0");
    CHECK(cmdl[1] == "1");
    CHECK(cmdl[2] == "2");
    CHECK(cmdl[3] == "3");
    CHECK(cmdl[4] == "4");
    CHECK(cmdl[argc + 10].empty());

    int val = -1;
    CHECK((cmdl(0) >> val));
    CHECK(val == 0);
    CHECK((cmdl(1) >> val));
    CHECK(val == 1);
    CHECK((cmdl(2) >> val));
    CHECK(val == 2);
    CHECK((cmdl(3) >> val));
    CHECK(val == 3);
    CHECK((cmdl(4) >> val));
    CHECK(val == 4);
    CHECK(!(cmdl(5) >> val));
    CHECK(val == 4);
}

TEST_CASE("Test flag access")
{
    parser cmdl;
    char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv);
    CHECK(2 == cmdl.flags_count());
    CHECK(5 == cmdl.size());


    CHECK(cmdl["a"]);
    CHECK(cmdl["b"]);
    CHECK(!cmdl["c"]);
}

TEST_CASE("Test parameter access")
{
    parser cmdl;
    char* argv[] = { "0", "-a", "-1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
    CHECK(2 == cmdl.params_count());
    CHECK(3 == cmdl.size());

    CHECK(cmdl("a").str() == "-1");
    CHECK(cmdl("b").str() == "2");
}

TEST_CASE("Test negative numbers are not options")
{
    char* argv[] = { "-1", "-0", "-0.4", "-1e6", "-1.3e-2" };
    int argc = sizeof(argv)/sizeof(argv[0]);
    parser cmdl;
    cmdl.parse(argc, argv);
    CHECK(argc == cmdl.size());
    CHECK(0 == cmdl.params_count());
    CHECK(0 == cmdl.flags_count());
}

TEST_CASE("Test default value technique")
{
    char* argv[] = { "-int", "-99999999999", "-double", "-1.3444444444e-2", "-string", "Hello" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    parser cmdl;
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);

    int v_int = -1;    
    double v_dbl = -1;
    
    CHECK(!(cmdl("-int") >> v_int));
    CHECK(-1 == v_int);

    CHECK(!(cmdl("-string") >> v_int));
    CHECK(-1 == v_int);

    CHECK(!(cmdl("-XXXXX") >> v_int));
    CHECK(-1 == v_int);

    CHECK(!(cmdl("-string") >> v_dbl));
    CHECK(-1 == v_dbl);

    CHECK(!(cmdl("-XXXXX") >> v_dbl));
    CHECK(-1 == v_dbl);

}

TEST_CASE("Test un-reg option modes")
{
    char* argv[] = { "-d", "-f", "123", "-g", "456", "-e" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    {
        parser cmdl;
        cmdl.add_param("g"); // register '-g'
        cmdl.parse(argc, argv);
        CHECK(cmdl["f"]); // f unknown should default to flag       
        CHECK(cmdl("f").str().empty());
        CHECK(!cmdl["g"]);
        CHECK(cmdl("g").str() == "456");
        CHECK(cmdl["d"]);
        CHECK(cmdl["e"]);
    }
    {
        parser cmdl;
        cmdl.add_param("g"); // register '-g'
        cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
        CHECK(!cmdl["f"]);
        CHECK(!cmdl("f").str().empty());
        CHECK(cmdl("f").str() == "123");
        CHECK(!cmdl["g"]);
        CHECK(!cmdl("g").str().empty());
        CHECK(cmdl("g").str() == "456");
        CHECK(cmdl["d"]);
        CHECK(cmdl["e"]);
    }
    {
        parser cmdl;
        cmdl.add_param("d"); // register 'd' and 'e', but they still should become flags since they
        cmdl.add_param("e"); // are not followed by a non-option
        cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
        CHECK(cmdl["d"]);
        CHECK(cmdl["e"]);
    }
}



