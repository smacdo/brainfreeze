#include "bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>
#include <iostream>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("a default constructed instruction is a nop with zero parameter", "[instructions]")
{
    instruction_t instr;
    REQUIRE(OpcodeType::NoOperation == instr.opcode());
    REQUIRE(0 == instr.param());
}

TEST_CASE("an instruction created with only an opcode has the opcode set", "[instructions]")
{
    instruction_t i0{ OpcodeType::Write };
    REQUIRE(OpcodeType::Write == i0.opcode());

    instruction_t i1{ OpcodeType::Read };
    REQUIRE(OpcodeType::Read == i1.opcode());
}

TEST_CASE("an instruction created with only an opcode has parameter set to zero", "[instructions]")
{
    instruction_t instr(OpcodeType::Write);
    REQUIRE(0 == instr.param());
}

TEST_CASE("an instruction created with opcode and parameter has those values set", "[instructions]")
{
    instruction_t i0{ OpcodeType::PtrInc, 5 };
    REQUIRE(5 == i0.param());

    instruction_t i1{ OpcodeType::JumpBack, 42 };
    REQUIRE(42 == i1.param());
}

TEST_CASE("an instruction parameter can be changed after construction", "[instructions]")
{
    instruction_t instr(OpcodeType::NoOperation, 5);
    REQUIRE(5 == instr.param());

    instr.setParam(-50);
    REQUIRE(-50 == instr.param());

    instr.setParam(0);
    REQUIRE(0 == instr.param());
}

TEST_CASE("changing the parameter does not effect the instruction opcode", "[instructions]")
{
    instruction_t instr(OpcodeType::Read, 42);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(42 == instr.param());

    instr.setParam(96);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(96 == instr.param());

    instr.setParam(-100);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(-100 == instr.param());
}

TEST_CASE("minimum and maximum parameter value is preserved", "[instructions]")
{
    instruction_t instr(OpcodeType::Read, 0);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(0 == instr.param());

    instr.setParam(32767);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(32767 == instr.param());

    instr.setParam(-32768);

    REQUIRE(OpcodeType::Read == instr.opcode());
    REQUIRE(-32768 == instr.param());
}

TEST_CASE("can test an instruction to see what opcode it has", "[instructions]")
{
    instruction_t i0{ OpcodeType::Write };
    REQUIRE(i0.isA(OpcodeType::Write));
    REQUIRE_FALSE(i0.isA(OpcodeType::PtrInc));
    REQUIRE_FALSE(i0.isA(OpcodeType::EndOfStream));
    REQUIRE_FALSE(i0.isA(OpcodeType::NoOperation));

    instruction_t i1{ OpcodeType::MemInc };
    REQUIRE(i1.isA(OpcodeType::MemInc));
    REQUIRE_FALSE(i1.isA(OpcodeType::MemDec));
    REQUIRE_FALSE(i1.isA(OpcodeType::EndOfStream));
    REQUIRE_FALSE(i1.isA(OpcodeType::NoOperation));
}

TEST_CASE("can increment a parameter", "[instructions]")
{
    instruction_t i;
    REQUIRE(0 == i.param());

    i.incrementParam(3);
    REQUIRE(3 == i.param());

    i.incrementParam(2);
    REQUIRE(5 == i.param());

    i.incrementParam(-4);
    REQUIRE(1 == i.param());
}

TEST_CASE("increment will throw an exception if incremented past max", "[instructions]")
{
    REQUIRE_THROWS_AS([&]() {
        instruction_t i;
        i.setParam(32767);
        i.incrementParam(1);
    }(), std::overflow_error);
}

TEST_CASE("increment will wrap if decremented past min", "[instructions]")
{
    REQUIRE_THROWS_AS([&]() {
        instruction_t i;
        i.setParam(-32768);
        i.incrementParam(-1);
        }(), std::underflow_error);
}
