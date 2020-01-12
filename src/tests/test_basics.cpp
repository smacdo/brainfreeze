#include "bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

Interpreter compile(const std::string& code)
{
    Compiler compiler;
    return Interpreter(compiler.compile(code));
}

TEST_CASE("no operations", "[ops]")
{
    auto app = compile(std::string( "" ));
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 0));
    REQUIRE_THAT(app, InstructionPointerIs(0));
    REQUIRE_THAT(app, MemoryPointerIs(0));
}

TEST_CASE("add op", "[ops]")
{
    auto app = compile( std::string("+") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 1));
}

TEST_CASE("sub op", "[ops]")
{
    auto app = compile( std::string("-") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, -1));
}

TEST_CASE("left op", "[ops]")
{
    auto app = compile( std::string(">") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 0));
    REQUIRE_THAT(app, MemoryPointerIs(1));
}

TEST_CASE("right op", "[ops]")
{
    auto app = compile( std::string(">><") );
    app.run();

    REQUIRE_THAT(app, MemoryPointerIs(1));
}

TEST_CASE("clear", "[programs]")
{
    auto app = compile( std::string("+++++[-]") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 0));
}

TEST_CASE("clear all cells", "[programs]")
{
    auto app = compile( std::string(">+++++>++++>+++>++>+[[-]<]") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 0));
    REQUIRE_THAT(app, HasMemory(1, 0));
    REQUIRE_THAT(app, HasMemory(2, 0));
    REQUIRE_THAT(app, HasMemory(3, 0));
    REQUIRE_THAT(app, HasMemory(4, 0));
}

TEST_CASE("rewind", "[programs]")
{
    auto app = compile( std::string(">>+>+[<]>") );
    app.run();

    REQUIRE_THAT(app, MemoryPointerIs(2));
}

TEST_CASE("fast forward", "[programs]")
{
    // 11101
    auto app = compile( std::string("+>+>+>>+<<<<[>]<") );
    app.run();

    REQUIRE_THAT(app, MemoryPointerIs(2));
}

TEST_CASE("destructive add", "[examples]")
{
    // 3 + 2, 5
    auto app = compile( std::string("+++>++<[->+<]") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 0));
    REQUIRE_THAT(app, HasMemory(1, 5));
}

TEST_CASE("nondestructive add", "[examples]")
{
    // 3 + 2, 5
    auto app = compile( std::string("+++>++<[->+>+<<]>>[-<<+>>]") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 3));
    REQUIRE_THAT(app, HasMemory(1, 5));
}

TEST_CASE("nondestructive copy", "[examples]")
{
    // 4 <-> 2
    auto app = compile( std::string("++++>++<>[-]>[-]<<[->+>+<<]>>[-<<+>>]<<") );
    app.run();

    REQUIRE_THAT(app, HasMemory(0, 4));
    REQUIRE_THAT(app, HasMemory(0, 4));
}
