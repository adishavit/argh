#include "argh.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace argh;

TEST_CASE("Test empty cmdl") 
{
    parser cmdl;
    cmdl.parse(0, nullptr);
    CHECK(0 == cmdl.pos_args().size());
    CHECK(cmdl[0].empty());
    CHECK(cmdl(0).str().empty());
    CHECK(cmdl[10].empty());
    CHECK(cmdl(10).str().empty());
    CHECK(!cmdl["xxx"]);
    CHECK(cmdl("xxx").str().empty());
}

TEST_CASE("Test parsing ctor")
{
   const char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
   int argc = sizeof(argv) / sizeof(argv[0]);
   {
      parser cmdl(argc, argv);
      CHECK(2 == cmdl.flags().size());
      CHECK(5 == cmdl.pos_args().size());
      CHECK(cmdl["a"]);
      CHECK(cmdl["b"]);
      CHECK(!cmdl["c"]);
   }
   {
      auto cmdl = parser(argc, argv);
      CHECK(2 == cmdl.flags().size());
      CHECK(5 == cmdl.pos_args().size());
      CHECK(cmdl["a"]);
      CHECK(cmdl["b"]);
      CHECK(!cmdl["c"]);
   }
}

TEST_CASE("Test positional access")
{
    parser cmdl; 
    const char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv);
    CHECK(5 == cmdl.pos_args().size());
    for (auto parg : cmdl.pos_args())
        CHECK(!parg.empty());
    
    CHECK(cmdl[0] == "0");
    CHECK(cmdl[1] == "1");
    CHECK(cmdl[2] == "2");
    CHECK(cmdl[3] == "3");
    CHECK(cmdl[4] == "4");
    CHECK(cmdl[argc + 10].empty());

    CHECK(cmdl(0));
    CHECK(cmdl(1));
    CHECK(cmdl(2));
    CHECK(cmdl(3));
    CHECK(cmdl(4));
    CHECK(!cmdl(argc + 10));

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
    const char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv);
    CHECK(2 == cmdl.flags().size());
    CHECK(5 == cmdl.pos_args().size());


    CHECK(cmdl["a"]);
    CHECK(cmdl["b"]);
    CHECK(!cmdl["c"]);
}

TEST_CASE("Test parameter access")
{
    parser cmdl;
    const char* argv[] = { "0", "-a", "-1", "-b", "2", "3", "4" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);
    CHECK(2 == cmdl.params().size());
    CHECK(3 == cmdl.pos_args().size());

    CHECK(cmdl("a").str() == "-1");
    CHECK(cmdl("b").str() == "2");
}

TEST_CASE("Test negative numbers are not options")
{
    const char* argv[] = { "-1", "-0", "-0.4", "-1e6", "-1.3e-2" };
    int argc = sizeof(argv)/sizeof(argv[0]);
    parser cmdl;
    cmdl.parse(argc, argv);
    CHECK(argc == cmdl.pos_args().size());
    CHECK(0 == cmdl.params().size());
    CHECK(0 == cmdl.flags().size());
}

TEST_CASE("Test failed istream access")
{
    const char* argv[] = { "-string", "Hello" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    parser cmdl;
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);

    int v_int = -1;    
    double v_dbl = -1;
    
    CHECK(!(cmdl("-string") >> v_int));
    CHECK((-1 == v_int || 0 == v_int)); // pre-C++11 vs post-C++11, see http://goo.gl/H6T2Ow

    CHECK(!(cmdl("-XXXXX") >> v_int));
    CHECK((-1 == v_int || 0 == v_int));

    CHECK(!(cmdl("-string") >> v_dbl));
    CHECK((-1 == v_int || 0 == v_int));

    CHECK(!(cmdl("-XXXXX") >> v_dbl));
    CHECK((-1 == v_int || 0 == v_int));
}

TEST_CASE("Test un-reg option modes")
{
    const char* argv[] = { "-d", "-f", "123", "-g", "456", "-e" };
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

TEST_CASE("Test default value")
{
    parser cmdl;
    const char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4", "A", "-c", "B" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);

    int val = -1;
    CHECK((cmdl(0, 7) >> val));
    CHECK(0 == val);
    CHECK((cmdl(argc+1, 7) >> val));
    CHECK(7 == val);
    CHECK((cmdl(argc + 1, "7") >> val)); // check default kicks in 
    CHECK(7 == val);

    val = -1;
    CHECK(!(cmdl(3, "7") >> val)); // this is an invalid conversion, no default. input error.
    CHECK((-1 == val || 0 == val));

    val = -1;
    CHECK((cmdl("XXX", 7) >> val));
    CHECK(7 == val);
    CHECK((cmdl("XXX", "8") >> val));
    CHECK(8 == val);
    val = -1;
    CHECK(!(cmdl("XXX", "*") >> val));
    CHECK((-1 == val || 0 == val));

    CHECK((cmdl("a", 7) >> val));
    CHECK(1 == val);
    CHECK((cmdl("b", 7) >> val));
    CHECK(2 == val);

    val = -1;
    CHECK(!(cmdl("c", 7) >> val)); // bad conversion
    CHECK((-1 == val || 0 == val));
    val = -1;
    CHECK(!(cmdl("c", "bad-default") >> val));
    CHECK((-1 == val || 0 == val));
}

TEST_CASE("Leading dashed are stripped")
{
    parser cmdl;
    const char* argv[] = { "-x", "--y", "---z", "-----------w" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv);
    CHECK(cmdl["x"]);
    CHECK(cmdl["-x"]);
    CHECK(cmdl["--x"]);
    CHECK(cmdl["---x"]);

    CHECK(cmdl["y"]);
    CHECK(cmdl["-y"]);
    CHECK(cmdl["--y"]);
    CHECK(cmdl["---y"]);

    CHECK(cmdl["z"]);
    CHECK(cmdl["-z"]);
    CHECK(cmdl["--z"]);
    CHECK(cmdl["---z"]);

    CHECK(cmdl["w"]);
    CHECK(cmdl["-w"]);
    CHECK(cmdl["--w"]);
    CHECK(cmdl["---w"]);
}

TEST_CASE("Split parameter at '='")
{
    {
        parser cmdl;
        const char* argv[] = { "--answer=42", "---no_val=" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        cmdl.parse(argc, argv);
        CHECK(!cmdl["answer"]);
        CHECK(cmdl("answer").str() == "42");
        CHECK(!cmdl["no_val"]);
        CHECK(cmdl("no_val").str() == "");
        CHECK(cmdl("no_val").str().empty());
    }
    {
        parser cmdl;
        const char* argv[] = { "--answer=42" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        cmdl.parse(argc, argv, parser::NO_SPLIT_ON_EQUALSIGN | parser::PREFER_FLAG_FOR_UNREG_OPTION);
        CHECK(!cmdl["answer"]);
        CHECK(cmdl["answer=42"]);
        CHECK(!(cmdl("answer").str() == "42"));
        CHECK(cmdl("answer=42").str().empty());
    }
}

TEST_CASE("Test empty stream")
{
    parser cmdl;
    const char* argv[] = { "--answer", "42", "-got_eq=pi", "-empty_eq=" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);

    // there are many way to check if the parameter is valid or not:
    CHECK(cmdl("answer"));
    CHECK(cmdl("got_eq"));
    CHECK(cmdl("empty_eq"));
    CHECK(cmdl("empty_eq"));
    CHECK(!cmdl("xxxxxx"));
    CHECK(!cmdl("xxxxxx"));

    // get the underlying string, this assumes an empty string is not possible as input (which is true unless using '--answer=')
    CHECK(cmdl("answer").str() == "42");
    CHECK(cmdl("got_eq").str() == "pi");
    CHECK(cmdl("empty_eq").str() == "");
    CHECK(cmdl("empty_eq").str().empty());
    CHECK(cmdl("xxxxxx").str() == "");
    CHECK(cmdl("xxxxxx").str().empty());

    // stream the value into a variable and check the stream state
    std::string val;
    CHECK( (cmdl("answer") >> val));
    CHECK( (cmdl("got_eq") >> val));
    CHECK(!(cmdl("xxxxxx") >> val));
    CHECK(!(cmdl("empty_eq") >> val));

    // access the stream rdbuf available chars to read directly and avoid unnecessary streaming
    CHECK(0  < cmdl("answer").rdbuf()->in_avail());
    CHECK(0  < cmdl("got_eq").rdbuf()->in_avail());
    CHECK(0 == cmdl("empty_eq").rdbuf()->in_avail());
    CHECK(0 == cmdl("xxxxxx").rdbuf()->in_avail());

    // check the stream state directly
    CHECK( cmdl("got_eq"));
    CHECK( cmdl("answer"));
    CHECK( cmdl("empty_eq")); // this will return true but an empty string.
    CHECK(!cmdl("xxxxxx"));
}

TEST_CASE("Interpret single-dash arg as multi-flag")
{
    const char* argv[] = { "-xvf", "42", "--abc", "54" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    {
        parser cmdl;
        cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION | parser::SINGLE_DASH_IS_MULTIFLAG);

        CHECK(cmdl["x"]);
        CHECK(cmdl["v"]);
        CHECK(cmdl["f"]);

        CHECK(!cmdl["a"]);
        CHECK(!cmdl["b"]);
        CHECK(!cmdl["c"]);
    }
    {
        parser cmdl;
        cmdl.parse(argc, argv);

        CHECK(cmdl["xvf"]);
        CHECK(cmdl["abc"]);
        CHECK(!cmdl["x"]);
        CHECK(!cmdl["v"]);
        CHECK(!cmdl["f"]);
        CHECK(!cmdl["a"]);
        CHECK(!cmdl["b"]);
        CHECK(!cmdl["c"]);
    }
    {
        parser cmdl;
        cmdl.parse(argc, argv, parser::PREFER_PARAM_FOR_UNREG_OPTION);

        CHECK(cmdl("xvf"));
        CHECK(cmdl("abc"));
        CHECK(!cmdl["x"]);
        CHECK(!cmdl["v"]);
        CHECK(!cmdl["f"]);
        CHECK(!cmdl["a"]);
        CHECK(!cmdl["b"]);
        CHECK(!cmdl["c"]);
    }
}


TEST_CASE("Handles const char versions as expected")
{
   {    
      const char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
      int argc = sizeof(argv) / sizeof(argv[0]);
      auto cmdl = parser(argc, argv);
      CHECK(5 == cmdl.pos_args().size());
      CHECK(2 == cmdl.flags().size());
   }
   {    
      char* argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
      int argc = sizeof(argv) / sizeof(argv[0]);
      auto cmdl = parser(argc, argv);
      CHECK(5 == cmdl.pos_args().size());
      CHECK(2 == cmdl.flags().size());
   }
   {
      char * const argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
      int argc = sizeof(argv) / sizeof(argv[0]);
      auto cmdl = parser(argc, argv);
      CHECK(5 == cmdl.pos_args().size());
      CHECK(2 == cmdl.flags().size());
   }
   {
      char const* const argv[] = { "0", "-a", "1", "-b", "2", "3", "4" };
      int argc = sizeof(argv) / sizeof(argv[0]);
      auto cmdl = parser(argc, argv);
      CHECK(5 == cmdl.pos_args().size());
      CHECK(2 == cmdl.flags().size());
   }
}