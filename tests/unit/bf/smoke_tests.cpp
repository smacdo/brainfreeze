#include "bf/bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("Hello World", "[smoketests]")
{
    std::string buffer;
    auto app = CreateInterpreter(
        "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.",
        []() { return Interpreter::byte_t{}; },
        [&buffer](Interpreter::byte_t b) { buffer.append(1, (char)b); });

    app.run();

    REQUIRE("Hello World!\n" == buffer);
}

TEST_CASE("Echo", "[smoketests]")
{
    std::string input("testing 123");
    std::string output;

    auto readFunc = [&input]() {
        if (input.empty())
        {
            return (Interpreter::byte_t)0;
        }
        else
        {
            auto c = input.front();
            input.erase(input.begin());
            return (Interpreter::byte_t)c;
        }};
    auto writeFunc = [&output](Interpreter::byte_t b) { output.append(1, (char)b); };
    auto app = CreateInterpreter(",[.,]", readFunc, writeFunc);
   
    app.run();

    REQUIRE("testing 123" == output);
}

TEST_CASE("Clear cell to zero", "[smoketests]")
{
    auto app = CreateInterpreter("+++++[-]");
    app.run();

    REQUIRE(0 == app.memoryAt(0));
}

TEST_CASE("Clear all cells to zero", "[smoketests]")
{
    auto app = CreateInterpreter(">+++++>++++>+++>++>+[[-]<]");
    app.run();

    REQUIRE(0 == app.memoryAt(0));
    REQUIRE(0 == app.memoryAt(1));
    REQUIRE(0 == app.memoryAt(2));
    REQUIRE(0 == app.memoryAt(3));
    REQUIRE(0 == app.memoryAt(4));
}

TEST_CASE("Rewind", "[smoketests]")
{
    auto app = CreateInterpreter(">>+>+[<]>");
    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(2));
}

TEST_CASE("Fast forward", "[smoketests]")
{
    // 11101
    auto app = CreateInterpreter("+>+>+>>+<<<<[>]<");
    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(2));
}

TEST_CASE("Destructive add", "[smoketests]")
{
    // 3 + 2, 5
    auto app = CreateInterpreter("+++>++<[->+<]");
    app.run();

    REQUIRE(0 == app.memoryAt(0));
    REQUIRE(5 == app.memoryAt(1));
}

TEST_CASE("Nondestructive add", "[smoketests]")
{
    // 3 + 2, 5
    auto app = CreateInterpreter("+++>++<[->+>+<<]>>[-<<+>>]");
    app.run();

    REQUIRE(3 == app.memoryAt(0));
    REQUIRE(5 == app.memoryAt(1));
}

TEST_CASE("Nondestructive copy", "[smoketests]")
{
    // 4 <-> 2
    //                                    >[-]>[-]<<[->+>+<<]>>[-<<+>>]<<
    auto app = CreateInterpreter("++++>++<>[-]>[-]<<[->+>+<<]>>[-<<+>>]<<");
    app.run();

    REQUIRE(4 == app.memoryAt(0));
    REQUIRE(4 == app.memoryAt(1));
}
