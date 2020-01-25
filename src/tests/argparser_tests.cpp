#include "argparser.h"
#include "../argparser/exceptions.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::ArgParsing;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("test if a simple flag was set", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help");
    ap.addOption("foobar");

    SECTION("is set if it is the only option")
    {
        const int ParamCount = 1;
        const char* Params[ParamCount] = { "--foobar" };

        ap.parse(ParamCount, Params);

        REQUIRE(ap.findOption("foobar").wasSet());
    }

    SECTION("is set if it is in the list of arguments given")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foobar", "--help", "--ohnoes" };

        ap.addOption("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.findOption("help").wasSet());
    }

    SECTION("can test if multiple flags are set")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foo", "--bar", "--foobar" };

        ap.addOption("foo");
        ap.addOption("bar");
        ap.addOption("fooba");
        ap.parse(ParamCount, Params);

        REQUIRE(ap.findOption("foobar").wasSet());
        REQUIRE(ap.findOption("bar").wasSet());
        REQUIRE(ap.findOption("foo").wasSet());

        REQUIRE_FALSE(ap.findOption("fooba").wasSet());
    }

    SECTION("is not set if it is not in the list of arguments given")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--foobar", "--help2", "--ohnoes" };

        ap.addOption("help2");
        ap.addOption("ohnoes");
        ap.parse(ParamCount, Params);

        REQUIRE_FALSE(ap.findOption("help").wasSet());
    }
}

TEST_CASE("an exception is thrown if an argument is not listed as a flag", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help");

    const int ParamCount = 1;
    const char* Params[ParamCount] = { "--foobar" };

    REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownLongNameException);
}

TEST_CASE("can specify a parameter that takes one string argument", "[argparser]")
{
    ArgParser ap;
    
    const int ParamCount = 2;
    const char* Params[ParamCount] = { "--filename", "test1.txt" };

    ap.addOption("filename").expectsArguments(1);
    ap.parse(ParamCount, Params);

    auto args = ap.findOption("filename").arguments();
    
    REQUIRE(1 == args.size());
    REQUIRE("test1.txt" == args[0]);
}

TEST_CASE("can specify a parameter that takes three string arguments", "[argparser]")
{
    ArgParser ap;

    const int ParamCount = 4;
    const char* Params[ParamCount] = { "--filename", "test1.txt", "test2.txt", "test3.txt" };

    ap.addOption("filename").expectsArguments(3);
    ap.parse(ParamCount, Params);

    auto args = ap.findOption("filename").arguments();

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

        ap.addOption("filename").expectsArguments(1);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }

    SECTION("if some arguments are provided and two is expected")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "--filename", "test.png", "hello.png" };

        ap.addOption("filename").expectsArguments(2);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 2, Params); }(), ExpectedArgumentMissingException);
        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }
}

TEST_CASE("can specify multiple parameters that take arguments", "[argparser]")
{
    // TODO: Make one of these take two arguments
    // TODO: More test cases in here with different set ups to make sure it works
    ArgParser ap;

    ap.addOption("filename").expectsArguments(1);
    ap.addOption("something").expectsArguments(1);
    ap.addOption("names");
    ap.addOption("verbose");

    SECTION("two parameters with one argument each separated by a flag")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "--filename", "test1.txt", "--verbose", "--something", "adam" };

        ap.parse(ParamCount, Params);

        auto args = ap.findOption("filename").arguments();

        REQUIRE(1 == args.size());
        REQUIRE("test1.txt" == args[0]);

        args = ap.findOption("something").arguments();

        REQUIRE(1 == args.size());
        REQUIRE("adam" == args[0]);
    }
}

TEST_CASE("parameter callback each time it is encountered", "[argparser]")
{
    ArgParser ap;
    ap.addOption("extra1");
    ap.addOption("extra2");

    SECTION("one callback for one parameter")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--verbose", "--extra2", "--extra1" };

        int verboseCounter = 0;
        ap.addOption("verbose").onParsed([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(1 == verboseCounter);
    }

    SECTION("different callbacks for different parameters")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--verbose", "--extra2", "--verbose" };

        int verboseCounter = 0;
        ap.addOption("verbose").onParsed([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(2 == verboseCounter);
    }

    SECTION("callbacks each time parameter is seen")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--extra1", "--foo", "--bar", "--bar" };

        int fooCounter = 0;
        ap.addOption("foo").onParsed([&fooCounter]() {fooCounter++; });

        int barCounter = 0;
        ap.addOption("bar").onParsed([&barCounter]() {barCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(1 == fooCounter);
        REQUIRE(2 == barCounter);
    }
}

TEST_CASE("parameter argument invoked each time argument seen for parmeter", "[argparser]")
{
    ArgParser ap;
    ap.addOption("extra1");
    ap.addOption("extra2");

    SECTION("one callback for one argument")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "--test", "apple" };

        std::string valueSeen;
        ap.addOption("test")
            .expectsArguments(1)
            .onArgument([&valueSeen](std::string_view argument) { valueSeen = argument; });

        ap.parse(ParamCount, Params);

        REQUIRE("apple" == valueSeen);
    }

    SECTION("many callback for one argument")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--test", "grape", "pear", "peach" };

        std::vector<std::string> values;
        ap.addOption("test")
            .expectsArguments(3)
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
        ap.addOption("name").bindString(&person);

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
        ap.addOption("favorite-number").bindInt(&favoriteNumber);

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
