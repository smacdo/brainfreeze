#include "argparser.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("test if a simple flag was set", "[argparser]")
{
    ArgParser ap;
    ap.addFlag('h', "help");
    ap.addFlag("foobar");

    SECTION("is set if it is the only option")
    {
        const int ParamCount = 1;
        const char* Params[ParamCount] = { "--foobar" };

        ap.addFlag("foobar");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.isFlagSet("foobar"));
    }

    SECTION("is set if it is in the list of arguments given")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foobar", "--help", "--ohnoes" };

        ap.addFlag("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.isFlagSet("help"));
    }

    SECTION("can test if multiple flags are set")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foo", "--bar", "--foobar" };

        ap.addFlag("foo");
        ap.addFlag("bar");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.isFlagSet("foobar"));
        REQUIRE(ap.isFlagSet("bar"));
        REQUIRE(ap.isFlagSet("foo"));

        REQUIRE_FALSE(ap.isFlagSet("fooba"));
    }

    SECTION("is not set if it is not in the list of arguments given")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foobar", "--help2", "--ohnoes" };

        ap.addFlag("help2");
        ap.addFlag("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE_FALSE(ap.isFlagSet("help"));
    }
}

TEST_CASE("an exception is thrown if an argument is not listed as a flag", "[argparser]")
{
    ArgParser ap;
    ap.addFlag('h', "help");

    const int ParamCount = 1;
    const char* Params[ParamCount] = { "--foobar" };

    REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownParameterLongNameException);
}

TEST_CASE("can specify a parameter that takes one string argument", "[argparser]")
{
    ArgParser ap;
    
    const int ParamCount = 2;
    const char* Params[ParamCount] = { "--filename", "test1.txt" };

    ap.addParameter("filename");
    ap.parse(ParamCount, Params);

    auto args = ap.getArgumentsForParameter("filename");
    
    REQUIRE(1 == args.size());
    REQUIRE("test1.txt" == args[0]);
}

TEST_CASE("can specify a parameter that takes three string arguments", "[argparser]")
{
    ArgParser ap;

    const int ParamCount = 4;
    const char* Params[ParamCount] = { "--filename", "test1.txt", "test2.txt", "test3.txt" };

    ap.addParameter("filename", 3);
    ap.parse(ParamCount, Params);

    auto args = ap.getArgumentsForParameter("filename");

    REQUIRE(3 == args.size());
    REQUIRE("test1.txt" == args[0]);
    REQUIRE("test2.txt" == args[1]);
    REQUIRE("test3.txt" == args[2]);
}

TEST_CASE("throws an exception if parameter expected arguments ar emissing", "[argparser]")
{
    ArgParser ap;

    SECTION("if no arguments are provided and one is expected")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "--filename", "test.png" };

        ap.addParameter("filename", 1);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedParameterArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }

    SECTION("if some arguments are provided and two is expected")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--filename", "test.png", "hello.png" };

        ap.addParameter("filename", 2);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 2, Params); }(), ExpectedParameterArgumentMissingException);
        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedParameterArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }
}

TEST_CASE("can specify multiple parameters that take arguments", "[argparser]")
{
    // TODO: Make one of these take two arguments
    // TODO: More test cases in here with different set ups to make sure it works
    ArgParser ap;

    ap.addParameter("filename");
    ap.addParameter("something");
    ap.addParameter("names");
    ap.addFlag("verbose");

    SECTION("two parameters with one argument each separated by a flag")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "--filename", "test1.txt", "--verbose", "--something", "adam" };

        ap.parse(ParamCount, Params);

        auto args = ap.getArgumentsForParameter("filename");

        REQUIRE(1 == args.size());
        REQUIRE("test1.txt" == args[0]);

        args = ap.getArgumentsForParameter("something");

        REQUIRE(1 == args.size());
        REQUIRE("adam" == args[0]);
    }


}

// TODO: Exception with no arguments.
// TODO: Exception with too many arguments.
// TODO: Test two parameters with two arguments.

// TODO: Test if parse throws exception on null input
// TODO: Test to make sure extra args are ignored if argc is too small.
// TODO: Test that nullptr is ignored if argc == 0
