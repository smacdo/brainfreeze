#include "argparser.h"
#include "../argparser/exceptions.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::ArgParsing;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("extract program name from first argument on command line", "[argparser]")
{
    ArgParser ap;

    SECTION("if it is the only argument on the command line")
    {
        const int ParamCount = 1;
        const char* Params[ParamCount] = { "myapp" };

        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->programName() == "myapp");
    }

    SECTION("when there are other options passed")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "foobar", "--filename", "test.txt", "--help" };

        ap.addOption("help");
        ap.addOption("filename").expectsArguments(1);

        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->programName() == "foobar");
    }

    SECTION("is ignored when options are parsed")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--not-an-option", "--filename", "test.txt", "--help" };

        ap.addOption("help");
        ap.addOption("filename").expectsArguments(1);
        ap.addOption("not-an-option");

        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->programName() == "--not-an-option");
    }

    SECTION("is ignored if extractProgramName=false")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "--not-an-option", "--filename", "test.txt", "--help" };

        ap.addOption("help");
        ap.addOption("filename").expectsArguments(1);
        ap.addOption("not-an-option");

        auto r = ap.parse(ParamCount, Params, false);

        REQUIRE(r->programName() == "");
    }
}

TEST_CASE("test if a simple flag was set with long names", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help");
    ap.addOption("foobar");

    SECTION("is set if it is the only option")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "--foobar" };

        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->option("foobar").wasSet());
    }

    SECTION("is set if it is in the list of arguments given")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "myapp", "--foobar", "--help", "--ohnoes" };

        ap.addOption("ohnoes");
        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->option("help").wasSet());
    }

    SECTION("can test if multiple flags are set")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "myapp", "--foo", "--bar", "--foobar" };

        ap.addOption("foo");
        ap.addOption("bar");
        ap.addOption("fooba");
        auto r = ap.parse(ParamCount, Params);

        REQUIRE(r->option("foobar").wasSet());
        REQUIRE(r->option("bar").wasSet());
        REQUIRE(r->option("foo").wasSet());

        REQUIRE_FALSE(r->option("fooba").wasSet());
    }

    SECTION("is not set if it is not in the list of arguments given")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "myapp", "--foobar", "--help2", "--ohnoes" };

        ap.addOption("help2");
        ap.addOption("ohnoes");
        auto r = ap.parse(ParamCount, Params);

        REQUIRE_FALSE(r->option("help").wasSet());
    }
}

TEST_CASE("an exception is thrown if an argument is not listed as a flag", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help");

    const int ParamCount = 2;
    const char* Params[ParamCount] = { "myapp", "--foobar" };

    REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownLongNameException);
}

TEST_CASE("test if a flag was set with short names", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help").shortName('h');
    ap.addOption("version").shortName('v');
    ap.addOption("force").shortName('F');

    SECTION("can test if single option (help) is set")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "-v" };

        auto r = ap.parse(ParamCount, Params);

        REQUIRE_FALSE(r->option("help").wasSet());
        REQUIRE(r->option("version").wasSet());
        REQUIRE_FALSE(r->option("force").wasSet());
    }

    SECTION("can test if single option (force) is set")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "-F" };

        auto r = ap.parse(ParamCount, Params);

        REQUIRE_FALSE(r->option("help").wasSet());
        REQUIRE_FALSE(r->option("version").wasSet());
        REQUIRE(r->option("force").wasSet());
    }

    SECTION("can test if multiple options are set (one per argument)")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "myapp", "-v", "-F", "-h" };

        auto a = ap.parse(2, Params);

        REQUIRE_FALSE(a->option("help").wasSet());
        REQUIRE(a->option("version").wasSet());
        REQUIRE_FALSE(a->option("force").wasSet());

        auto b = ap.parse(3, Params);

        REQUIRE_FALSE(b->option("help").wasSet());
        REQUIRE(b->option("version").wasSet());
        REQUIRE(b->option("force").wasSet());

        auto c = ap.parse(4, Params);

        REQUIRE(c->option("help").wasSet());
        REQUIRE(c->option("version").wasSet());
        REQUIRE(c->option("force").wasSet());
    }
}

// TODO: -vf
// TODO: -vfx [argument]
// TODO: -vxy [argument] where x and y except argument => EXCEPTION
// TODO: -vxf [argument] where x expect argument => EXCEPTION

TEST_CASE("an exception is thrown if a short name is not registered", "[argparser]")
{
    ArgParser ap;
    ap.addOption("help").shortName('h');
    ap.addOption("version").shortName('v');
    ap.addOption("force").shortName('F');

    SECTION("for lowercase force")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "-f" };

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownShortNameException);
    }

    SECTION("for non-registered char")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "-x" };

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount, Params); }(), UnknownShortNameException);
    }
}

TEST_CASE("can specify a parameter that takes one string argument", "[argparser]")
{
    ArgParser ap;
    
    const int ParamCount = 3;
    const char* Params[ParamCount] = { "myapp", "--filename", "test1.txt" };

    ap.addOption("filename").expectsArguments(1);
    auto r = ap.parse(ParamCount, Params);

    SECTION("get options via arguments method")
    {
        auto args = r->option("filename").arguments();

        REQUIRE(1 == args.size());
        REQUIRE("test1.txt" == args[0]);
    }

    SECTION("get options via argumentValue method")
    {
        auto fileName = r->option("filename").argumentValue();
        REQUIRE("test1.txt" == fileName);
    }
}

TEST_CASE("can specify a parameter that takes three string arguments", "[argparser]")
{
    ArgParser ap;

    const int ParamCount = 5;
    const char* Params[ParamCount] = { "myapp", "--filename", "test1.txt", "test2.txt", "test3.txt" };

    ap.addOption("filename").expectsArguments(3);
    auto r = ap.parse(ParamCount, Params);

    auto args = r->option("filename").arguments();

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
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "--filename", "test.png" };

        ap.addOption("filename").expectsArguments(1);

        REQUIRE_THROWS_AS([&]() { ap.parse(ParamCount - 1, Params); }(), ExpectedArgumentMissingException);
        REQUIRE_NOTHROW([&]() { ap.parse(ParamCount, Params); }());
    }

    SECTION("if some arguments are provided and two is expected")
    {
        const int ParamCount = 4;
        const char* Params[ParamCount] = { "myapp", "--filename", "test.png", "hello.png" };

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
        const int ParamCount = 6;
        const char* Params[ParamCount] = { "myapp", "--filename", "test1.txt", "--verbose", "--something", "adam" };

        auto r = ap.parse(ParamCount, Params);

        auto args = r->option("filename").arguments();

        REQUIRE(1 == args.size());
        REQUIRE("test1.txt" == args[0]);

        args = r->option("something").arguments();

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
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "myapp", "--extra1", "--verbose", "--extra2", "--extra1" };

        int verboseCounter = 0;
        ap.addOption("verbose").onParsed([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(1 == verboseCounter);
    }

    SECTION("different callbacks for different parameters")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "myapp", "--extra1", "--verbose", "--extra2", "--verbose" };

        int verboseCounter = 0;
        ap.addOption("verbose").onParsed([&verboseCounter]() {verboseCounter++; });

        ap.parse(ParamCount, Params);

        REQUIRE(2 == verboseCounter);
    }

    SECTION("callbacks each time parameter is seen")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "myapp", "--extra1", "--foo", "--bar", "--bar" };

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
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "--test", "apple" };

        std::string valueSeen;
        ap.addOption("test")
            .expectsArguments(1)
            .onArgument([&valueSeen](std::string_view argument) { valueSeen = argument; });

        ap.parse(ParamCount, Params);

        REQUIRE("apple" == valueSeen);
    }

    SECTION("many callback for one argument")
    {
        const int ParamCount = 5;
        const char* Params[ParamCount] = { "myapp", "--test", "grape", "pear", "peach" };

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

TEST_CASE("Specify positional parameters without option names", "[argparser]")
{
    ArgParser ap;

    SECTION("one positional parameter to bind", "[argparser]")
    {
        const int ParamCount = 2;
        const char* Params[ParamCount] = { "myapp", "foobar.txt" };

        std::string filename;
        ap.addOption("filename").expectsArguments(1).positional().bindString(&filename); // TODO: Rewrite with expectArgument(T).

        auto r = ap.parse(ParamCount, Params);

        REQUIRE("foobar.txt" == filename);
        REQUIRE(r->option("filename").arguments()[0] == "foobar.txt");
        REQUIRE(r->option("filename").wasSet());
    }

    SECTION("two positional parameters to bind", "[argparser]")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "readthis.txt", "wrotethat.txt" };

        std::string inputFileName;
        std::string outputFileName;

        ap.addOption("input").expectsArguments(1).positional().bindString(&inputFileName); // TODO: Rewrite with expectArgument(T).
        ap.addOption("output").expectsArguments(1).positional().bindString(&outputFileName);

        auto r = ap.parse(ParamCount, Params);

        REQUIRE("readthis.txt" == inputFileName);
        REQUIRE(r->option("input").arguments()[0] == "readthis.txt");
        REQUIRE(r->option("input").wasSet());

        REQUIRE("wrotethat.txt" == outputFileName);
        REQUIRE(r->option("output").arguments()[0] == "wrotethat.txt");
        REQUIRE(r->option("output").wasSet());
    }

    SECTION("two positional parameters out of order", "[argparser]")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "abcdefg", "123456" };

        std::string first;
        std::string second;

        ap.addOption("second").expectsArguments(1).positional(1).bindString(&second);
        ap.addOption("first").expectsArguments(1).positional(0).bindString(&first);

        auto r = ap.parse(ParamCount, Params);

        REQUIRE("abcdefg" == first);
        REQUIRE(r->option("first").arguments()[0] == "abcdefg");
        REQUIRE(r->option("first").wasSet());

        REQUIRE("123456" == second);
        REQUIRE(r->option("second").arguments()[0] == "123456");
        REQUIRE(r->option("second").wasSet());
    }

    // TODO: Intersperse short and long name parameters with an without options to test.
    // TODO: Test absence of optional positional => NOT SET.
    // TODO: Test create two positionals out of order (first #1, then #0).
}

TEST_CASE("string parameter binding", "[argparser]")
{
    ArgParser ap;

    SECTION("one binding for one argument")
    {
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "--name", "christine" };

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
        const int ParamCount = 3;
        const char* Params[ParamCount] = { "myapp", "--favorite-number", "42" };

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
