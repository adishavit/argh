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


