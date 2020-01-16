#include "argparser.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("test if a simple flag was set", "[argparser]")
{
    ArgParser ap;
    ap.addFlagParameter("help");
    ap.addFlagParameter("foobar");

    SECTION("is set if it is the only option")
    {
        const int ParamCount = 1;
        const char* Params[ParamCount] = { "--foobar" };

        ap.addFlagParameter("foobar");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.isFlagSet("foobar"));
    }

    SECTION("is set if it is in the list of arguments given")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foobar", "--help", "--ohnoes" };

        ap.addFlagParameter("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.isFlagSet("help"));
    }

    SECTION("can test if multiple flags are set")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foo", "--bar", "--foobar" };

        ap.addFlagParameter("foo");
        ap.addFlagParameter("bar");
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

        ap.addFlagParameter("help2");
        ap.addFlagParameter("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE_FALSE(ap.isFlagSet("help"));
    }
}

TEST_CASE("an exception is thrown if an argument is not listed as a flag", "[argparser]")
{
    ArgParser ap;
    ap.addFlagParameter("help");

    const int ParamCount = 1;
    const char* Params[ParamCount] = { "--foobar" };

    REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownParameterLongNameException);
}

TEST_CASE("can specify a parameter that takes one string argument", "[argparser]")
{
    ArgParser ap;
    
    const int ParamCount = 2;
    const char* Params[ParamCount] = { "--filename", "test1.txt" };

    ap.addParameter("filename").expectedArgumentCount(1);
    ap.parse(ParamCount, Params);

    auto args = ap.parameterArguments("filename");
    
    REQUIRE(1 == args.size());
    REQUIRE("test1.txt" == args[0]);
}

TEST_CASE("can specify a parameter that takes three string arguments", "[argparser]")
{
    ArgParser ap;

    const int ParamCount = 4;
    const char* Params[ParamCount] = { "--filename", "test1.txt", "test2.txt", "test3.txt" };

    ap.addParameter("filename").expectedArgumentCount(3);
    ap.parse(ParamCount, Params);

    auto args = ap.parameterArguments("filename");

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

        ap.addParameter("filename").expectedArgumentCount(1);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedParameterArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }

    SECTION("if some arguments are provided and two is expected")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--filename", "test.png", "hello.png" };

        ap.addParameter("filename").expectedArgumentCount(2);

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

    ap.addParameter("filename").expectedArgumentCount(1);
    ap.addParameter("something").expectedArgumentCount(1);
    ap.addParameter("names");
    ap.addFlagParameter("verbose");

    SECTION("two parameters with one argument each separated by a flag")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "--filename", "test1.txt", "--verbose", "--something", "adam" };

        ap.parse(ParamCount, Params);

        auto args = ap.parameterArguments("filename");

        REQUIRE(1 == args.size());
        REQUIRE("test1.txt" == args[0]);

        args = ap.parameterArguments("something");

        REQUIRE(1 == args.size());
        REQUIRE("adam" == args[0]);
    }
}

TEST_CASE("parameter callback each time it is encountered", "[argparser]")
{
    ArgParser ap;
    ap.addFlagParameter("extra1");
    ap.addFlagParameter("extra2");

    SECTION("one callback for one parameter")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--verbose", "--extra2", "--extra1" };

        int verboseCounter = 0;
        ap.addFlagParameter("verbose").onParam([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(1 == verboseCounter);
    }

    SECTION("different callbacks for different parameters")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--verbose", "--extra2", "--verbose" };

        int verboseCounter = 0;
        ap.addFlagParameter("verbose").onParam([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(2 == verboseCounter);
    }

    SECTION("callbacks each time parameter is seen")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--foo", "--bar", "--bar" };

        int fooCounter = 0;
        ap.addFlagParameter("foo").onParam([&fooCounter]() {fooCounter++; });

        int barCounter = 0;
        ap.addFlagParameter("bar").onParam([&barCounter]() {barCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(1 == fooCounter);
        REQUIRE(2 == barCounter);
    }
}

TEST_CASE("parameter argument invoked each time argument seen for parmeter", "[argparser]")
{
    ArgParser ap;
    ap.addFlagParameter("extra1");
    ap.addFlagParameter("extra2");

    SECTION("one callback for one argument")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "--test", "apple" };

        std::string valueSeen;
        ap.addFlagParameter("test")
            .expectedArgumentCount(1)
            .onArgument([&valueSeen](std::string_view argument) { valueSeen = argument; });

        ap.parse(ParamCount, Params);

        REQUIRE("apple" == valueSeen);
    }

    SECTION("many callback for one argument")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--test", "grape", "pear", "peach" };

        std::vector<std::string> values;
        ap.addFlagParameter("test")
            .expectedArgumentCount(3)
            .onArgument([&values](std::string_view argument) { values.push_back(std::string(argument)); });

        ap.parse(ParamCount, Params);

        std::vector<std::string> Expected = { "grape", "pear", "peach" };
        REQUIRE_THAT(values, Catch::Equals(Expected));
    }

    // TODO: Multiple callbacks for multipel arguments
}

TEST_CASE("string parameter binding", "[argparser]")
{
    ArgParser ap;

    SECTION("one binding for one argument")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "--name", "christine" };

        std::string person;
        ap.addParameter("name").bindString(&person);

        ap.parse(ParamCount, Params);

        REQUIRE("christine" == person);
    }

    // TODO: Test exception when binding to multiple expected arguments.
}

TEST_CASE("int parameter binding", "[argparser]")
{
    ArgParser ap;

    SECTION("one binding for one argument")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "--favorite-number", "42" };

        int favoriteNumber = 0;
        ap.addParameter("favorite-number").bindInt(&favoriteNumber);

        ap.parse(ParamCount, Params);

        REQUIRE(42 == favoriteNumber);
    }

    // TODO: Test exception when binding to multiple expected arguments.
}

// TODO: Test size_t binding.

// TODO: Exception with no arguments.
// TODO: Exception with too many arguments.
// TODO: Test two parameters with two arguments.

// TODO: Test if parse throws exception on null input
// TODO: Test to make sure extra args are ignored if argc is too small.
// TODO: Test that nullptr is ignored if argc == 0
