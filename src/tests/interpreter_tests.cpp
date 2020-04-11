#include "bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("empty program runs and does nothing", "[interpreter]")
{
    auto app = CreateInterpreter(std::string(""));
    app.run();

    REQUIRE_THAT(app.instructionPointer(), InstructionPointerIs(0));
    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
    REQUIRE(0u == app.memoryAt(0));
}

TEST_CASE("> increments the memory pointer", "[interpreter]")
{
    SECTION("with one increment")
    {
        auto app = CreateInterpreter(std::string(">"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
        REQUIRE(0u == app.memoryAt(0));
    }

    SECTION("with two increments")
    {
        auto app = CreateInterpreter(std::string(">>"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(2));
        REQUIRE(0u == app.memoryAt(0));
    }
}

TEST_CASE("< decrements the memory pointer", "[interpreter]")
{
    SECTION("with one decrement after incrementing forward")
    {
        auto app = CreateInterpreter(std::string(">><"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
        REQUIRE(0u == app.memoryAt(0));
    }

    SECTION("with two decrements after incrementing forward")
    {
        auto app = CreateInterpreter(std::string(">><<"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(0u == app.memoryAt(0));
    }
}

TEST_CASE("+ increments the value at the memory pointer location", "[interpreter]")
{
    SECTION("with one increment")
    {
        auto app = CreateInterpreter(std::string("+"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(1u == app.memoryAt(0));
    }

    SECTION("with two increments")
    {
        auto app = CreateInterpreter(std::string("++"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(2u == app.memoryAt(0));
    }

    SECTION("with one increment at the start and then two in the next cell")
    {
        auto app = CreateInterpreter(std::string("+>++"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
        REQUIRE(1u == app.memoryAt(0));
        REQUIRE(2u == app.memoryAt(1));
    }
}

TEST_CASE("- decrements the value at the memory pointer location", "[interpreter]")
{
    SECTION("with one decrement after first incrementing")
    {
        auto app = CreateInterpreter(std::string("+++-"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(2u == app.memoryAt(0));
    }

    SECTION("with two decrement after first incrementing")
    {
        auto app = CreateInterpreter(std::string("+++--"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(1u == app.memoryAt(0));
    }

    SECTION("with incrementing and then decrementing to zero")
    {
        auto app = CreateInterpreter(std::string("+-++--"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(0));
        REQUIRE(0u == app.memoryAt(0));
    }

    SECTION("with one decrement at the start and then two in the next cell after incrementing")
    {
        auto app = CreateInterpreter(std::string("++->++--+-"));
        app.run();

        REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
        REQUIRE(1u == app.memoryAt(0));
        REQUIRE(0u == app.memoryAt(1));
    }
}

TEST_CASE(", reads a byte and writes to the current memory location", "[interpreter]")
{
    Interpreter::byte_t counter = 42;
    
    auto app = CreateInterpreter(
        std::string(",>,"),
        [&counter]() { return counter++; },
        [](Interpreter::byte_t) {});
    
    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
    REQUIRE(42 == app.memoryAt(0));
    REQUIRE(43 == app.memoryAt(1));
}

TEST_CASE(". writes a byte from the current memory location", "[interpreter]")
{
    std::vector<Interpreter::byte_t> bytes;

    auto app = CreateInterpreter(
        std::string("++.>+++++.+"),
        []() { return Interpreter::byte_t{}; },
        [&bytes](Interpreter::byte_t v) { bytes.push_back(v); });

    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
    REQUIRE(2 == app.memoryAt(0));
    REQUIRE(6 == app.memoryAt(1));

    REQUIRE(2 == bytes.size());
    REQUIRE(2 == bytes[0]);
    REQUIRE(5 == bytes[1]);
}

TEST_CASE("[ jumps to ] if current byte is zero", "[interpreter]")
{
    auto app = CreateInterpreter(std::string("[++]>+"));
    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
    REQUIRE(0 == app.memoryAt(0));
    REQUIRE(1 == app.memoryAt(1));
}

TEST_CASE("] jumps to [ if current byte is not zero", "[interpreter]")
{
    auto app = CreateInterpreter(std::string("++[-]>+"));
    app.run();

    REQUIRE_THAT(app.memoryPointer(), MemoryPointerIs(1));
    REQUIRE(0 == app.memoryAt(0));
    REQUIRE(1 == app.memoryAt(1));
}
