#include "bf/bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("a program with zero characters is valid", "[compiler]")
{
    auto il = Compile("");

    REQUIRE(1 == il.size());
    REQUIRE(instruction_t(OpcodeType::EndOfStream) == il[0]);
}

TEST_CASE("a program is always terminated with a end of stream instruction", "[compiler]")
{
    SECTION("when there are no instructions")
    {
        auto il = Compile("");

        REQUIRE(1 == il.size());
        REQUIRE(instruction_t(OpcodeType::EndOfStream) == il[0]);
    }

    SECTION("when there is one instruction")
    {
        auto il = Compile("+");

        REQUIRE(2 == il.size());
        REQUIRE(instruction_t(OpcodeType::EndOfStream) == il[1]);
    }

    SECTION("when there are many instructions")
    {
        auto il = Compile("[-]");

        REQUIRE(4 == il.size());
        REQUIRE(instruction_t(OpcodeType::EndOfStream) == il[3]);
    }
}

TEST_CASE("the compiler ignores characters that are not brainfreeze instructions", "[compiler]")
{
    auto il = Compile("hello+ -world");

    REQUIRE(3 == il.size());
    REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[0]);
    REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[1]);
    REQUIRE(instruction_t(OpcodeType::EndOfStream) == il[2]);
}

TEST_CASE("> is compiled to a PTR_INC instruction", "[compiler]")
{
    auto il = Compile(">");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[0]);
}

TEST_CASE("< is compiled to a PTR_DEC instruction", "[compiler]")
{
    auto il = Compile("<");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[0]);
}

TEST_CASE("+ is compiled to a MEM_INC instruction", "[compiler]")
{
    auto il = Compile("+");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[0]);
}

TEST_CASE("- is compiled to a MEM_DEC instruction", "[compiler]")
{
    auto il = Compile("-");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[0]);
}

TEST_CASE(". is compiled to a WRITE instruction", "[compiler]")
{
    auto il = Compile(".");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::Write, 0) == il[0]);
}

TEST_CASE(", is compiled to a READ instruction", "[compiler]")
{
    auto il = Compile(",");

    REQUIRE(2 == il.size());
    REQUIRE(instruction_t(OpcodeType::Read, 0) == il[0]);
}

TEST_CASE("[] are compiled to JUMP_FORWARD and JUMP_BACK instructions", "[compiler]")
{
    auto il = Compile("[]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(false); });

    REQUIRE(3 == il.size());
    REQUIRE(instruction_t(OpcodeType::JumpForward, 0) == il[0]);
    REQUIRE(instruction_t(OpcodeType::JumpBack, 0) == il[1]);
}

TEST_CASE("> runs can be merged when the optimization flag is enabled", "[compiler]")
{
    SECTION("are not merged when there is only one instance")
    {
        auto il = Compile(">+>", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(4 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[3]);
    }

    SECTION("are merged when there are multiple instances")
    {
        //                 0 1234
        auto il = Compile(">>,>.>>>", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(6 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrInc, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[1]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[3]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 3) == il[4]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[5]);
    }

    SECTION("are merged across ignored non-instruction characters")
    {
        auto il = Compile("> >", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(2 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrInc, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[1]);
    }

    SECTION("are not merged when the optimization is disabled")
    {
        //                 01234578
        auto il = Compile(">>,>.>>>", [](Compiler& c) { c.setMergeInstructionsEnabled(false); });
        REQUIRE(9 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[2]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[3]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[4]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[5]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[6]);
        REQUIRE(instruction_t(OpcodeType::PtrInc, 1) == il[7]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[8]);
    }
}

TEST_CASE("< runs can be merged when the optimization flag is enabled", "[compiler]")
{
    SECTION("are not merged when there is only one instance")
    {
        auto il = Compile("<+<", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(4 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[3]);
    }

    SECTION("are merged when there are multiple instances")
    {
        //                 0 1234
        auto il = Compile("<<,<.<<<", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(6 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrDec, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[1]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[3]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 3) == il[4]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[5]);
    }

    SECTION("are merged across ignored non-instruction characters")
    {
        auto il = Compile("< <", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(2 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrDec, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[1]);
    }

    SECTION("are not merged when the optimization is disabled")
    {
        //                 01234578
        auto il = Compile("<<,<.<<<", [](Compiler& c) { c.setMergeInstructionsEnabled(false); });
        REQUIRE(9 == il.size());
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[2]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[3]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[4]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[5]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[6]);
        REQUIRE(instruction_t(OpcodeType::PtrDec, 1) == il[7]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[8]);
    }
}

TEST_CASE("+ runs can be merged when the optimization flag is enabled", "[compiler]")
{
    SECTION("are not merged when there is only one instance")
    {
        auto il = Compile("+-+", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(4 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[3]);
    }

    SECTION("are merged when there are multiple instances")
    {
        //                 0 1234
        auto il = Compile("++,+.+++", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(6 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemInc, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[1]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[3]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 3) == il[4]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[5]);
    }

    SECTION("are merged across ignored non-instruction characters")
    {
        auto il = Compile("+ +", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(2 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemInc, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[1]);
    }

    SECTION("are not merged when the optimization is disabled")
    {
        //                 01234578
        auto il = Compile("++,+.+++", [](Compiler& c) { c.setMergeInstructionsEnabled(false); });
        REQUIRE(9 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[2]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[3]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[4]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[5]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[6]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[7]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[8]);
    }
}

TEST_CASE("- runs can be merged when the optimization flag is enabled", "[compiler]")
{
    SECTION("are not merged when there is only one instance")
    {
        auto il = Compile("-+-", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(4 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemInc, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[3]);
    }

    SECTION("are merged when there are multiple instances")
    {
        //                 0 1234
        auto il = Compile("--,-.---", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(6 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemDec, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[1]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[2]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[3]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 3) == il[4]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[5]);
    }

    SECTION("are merged across ignored non-instruction characters")
    {
        auto il = Compile("- -", [](Compiler& c) { c.setMergeInstructionsEnabled(true); });
        REQUIRE(2 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemDec, 2) == il[0]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[1]);
    }

    SECTION("are not merged when the optimization is disabled")
    {
        //                 01234578
        auto il = Compile("--,-.---", [](Compiler& c) { c.setMergeInstructionsEnabled(false); });
        REQUIRE(9 == il.size());
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[0]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[1]);
        REQUIRE(instruction_t(OpcodeType::Read, 0) == il[2]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[3]);
        REQUIRE(instruction_t(OpcodeType::Write, 0) == il[4]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[5]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[6]);
        REQUIRE(instruction_t(OpcodeType::MemDec, 1) == il[7]);
        REQUIRE(instruction_t(OpcodeType::EndOfStream, 0) == il[8]);
    }
}

TEST_CASE("jump offsets are stored when the precalculate optimization is enabled", "[compiler]")
{
    //                 012345678
    auto il = Compile("[[.][]][]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(true); });

    REQUIRE(instruction_t(OpcodeType::FastJumpForward, 6 - 0) == il[0]);
    REQUIRE(instruction_t(OpcodeType::FastJumpForward, 3 - 1) == il[1]);
    REQUIRE(instruction_t(OpcodeType::FastJumpBack, 3 - 1) == il[3]);
    REQUIRE(instruction_t(OpcodeType::FastJumpForward, 5 - 4) == il[4]);
    REQUIRE(instruction_t(OpcodeType::FastJumpBack, 5 - 4) == il[5]);
    REQUIRE(instruction_t(OpcodeType::FastJumpBack, 6 - 0) == il[6]);
    REQUIRE(instruction_t(OpcodeType::FastJumpForward, 8 - 7) == il[7]);
    REQUIRE(instruction_t(OpcodeType::FastJumpBack, 8 - 7) == il[8]);
}

TEST_CASE("unbalanced jump characters will throw an exception", "[compiler]")
{
    REQUIRE_THROWS_WITH(
        [&]() { Compile("[", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(true); }); }(),
        "Unbalanced jump, expected a ] before program termination");
    REQUIRE_THROWS_WITH(
        [&]() { Compile("[", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(false); }); }(),
        "Unbalanced jump, expected a ] before program termination");

    REQUIRE_THROWS_WITH(
        [&]() { Compile("]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(true); }); }(),
        "Unbalanced jump, expected a [ before this ]");
    REQUIRE_THROWS_WITH(
        [&]() { Compile("]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(false); }); }(),
        "Unbalanced jump, expected a [ before this ]");

    REQUIRE_THROWS_WITH(
        [&]() { Compile("[[[]]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(true); }); }(),
        "Unbalanced jump, expected a ] before program termination");
    REQUIRE_THROWS_WITH(
        [&]() { Compile("[[[]]", [](Compiler& c) { c.setPrecalculateJumpOffsetsEnabled(false); }); }(),
        "Unbalanced jump, expected a ] before program termination");
}
