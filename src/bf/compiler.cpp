// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <stack>
#include <cassert>
#include <limits>
#include <stdexcept>

using namespace Brainfreeze;

// TODO: Maybe put this in a header for other poeple to use?
namespace Characters
{
    const char END_OF_STREAM = '\0';
    const char PTR_INC = '>';
    const char PTR_DEC = '<';
    const char MEM_INC = '+';
    const char MEM_DEC = '-';
    const char WRITE = '.';
    const char READ = ',';
    const char JMP_FWD = '[';
    const char JMP_BAC = ']';
}

//---------------------------------------------------------------------------------------------------------------------
std::vector<instruction_t> Compiler::compile(std::string_view programtext) const
{
    // TODO: Move the optimizations into an optimizer pass.
    std::vector<instruction_t> instructions;
    instructions.reserve(programtext.size());      // Over-reserve for speed, and release extra at the end of compile.

    // Track jump targets for optimization (And also report when unbalanced jumps are encountered).
    std::stack<size_t> jumps;

    // Convert each legal character in the programtext to its compiled brainfreeze instruction form.    
    for (auto c : programtext)
    {
        // Skip characters that are not legal brainfreeze instructions.
        if (!isInstruction(c))
        {
            continue;
        }

        // Calculate the index that will be used for the next instruction written into the program.
        auto nextIndex = instructions.size();

        // Get the instruction form for this character.
        auto instr = asInstruction(c);

        // Handle jump instructions specially. Jump forwards need to have their position recorded so that when the
        // matching backward jump is found both jumps can have their offset written into them for optimization.
        // TODO: Move this code into an optimization pass.
        if (instr.isA(OpcodeType::JumpForward))
        {
            // Record the jump forward position until the matching jumping backward instruction is located.
            jumps.push(nextIndex);
        }
        else if (instr.isA(OpcodeType::JumpBack))
        {
            // Backward jump found. Pop the top jump marker off the stack which is the matching forwrad jump target.
            // Write the offset of the jump into both the forward and backward jump instructions.

            // Get the offset of the matching forward [ jump.
            auto forwardJumpOffset = jumps.top();
            jumps.pop();

            auto distance = nextIndex - forwardJumpOffset;
            assert(distance > 0);

            // Verify the jump distance is small enough to fit in the instruction parameter.
            // TODO: How should we handle jump targets that are too big? Is this even a problem?
            if (distance > std::numeric_limits<instruction_t::param_t>::max())
            {
                // TODO: Should the size be included in the message?
                throw std::runtime_error("Jump target to large to fit in instruction");
            }

            // Store the offset in the [ instruction.
            instructions[forwardJumpOffset].setParam(static_cast<instruction_t::param_t>(distance));

            // Store the offset in the current ] instruction.
            instr.setParam(static_cast<instruction_t::param_t>(distance));
        }

        // Is this instruction a repeat of the previous instruction? If it is a repeating instruction that supports
        // merging (like increment/decrement) then merge it into the last instruction and increase the parameter
        // count.
        //
        // TODO: This is an optimization step that should be moved into an optimizer.
        if (isMergable(instr) &&                                    // is this instruction mergable?
            instructions.size() > 0 &&                              // are there any instructions already stored?
            instr.opcode() == instructions[nextIndex - 1].opcode()) // does this match previous instruction?
        {
            // Increment the last instruction's parameter. Do not insert this instruction.
            instructions[nextIndex - 1].incrementParam(1);
        }
        else
        {
            // Add this instruction to the program.
            instructions.push_back(instr);
        }
    }

    // Verify the jump stack is empty. If not, then there is an unmatched jump somewhere!
    if (!jumps.empty())
    {
        // TODO: Maybe hint?
        throw std::runtime_error("Unmatched jump target found when compiling");
    }

    // Insert end of program instruction.
    instructions.push_back(instruction_t(OpcodeType::EndOfStream));

    // Remove unused space from the list of instructions before returning.
    instructions.shrink_to_fit();
    return instructions;
}

//---------------------------------------------------------------------------------------------------------------------
bool Compiler::isInstruction(char c) noexcept
{
    return c == Characters::PTR_INC
        || c == Characters::PTR_DEC
        || c == Characters::MEM_INC
        || c == Characters::MEM_DEC
        || c == Characters::WRITE
        || c == Characters::READ
        || c == Characters::JMP_FWD
        || c == Characters::JMP_BAC;
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t Compiler::asInstruction(char c)
{
    switch (c)
    {
    case Characters::PTR_INC:
        return instruction_t(OpcodeType::PtrInc, 1);
    case Characters::PTR_DEC:
        return instruction_t(OpcodeType::PtrDec, 1);
    case Characters::MEM_INC:
        return instruction_t(OpcodeType::MemInc, 1);
    case Characters::MEM_DEC:
        return instruction_t(OpcodeType::MemDec, 1);
    case Characters::READ:
        return instruction_t(OpcodeType::Read, 0);
    case Characters::WRITE:
        return instruction_t(OpcodeType::Write, 0);
    case Characters::JMP_FWD:
        return instruction_t(OpcodeType::JumpForward, 0);
    case Characters::JMP_BAC:
        return instruction_t(OpcodeType::JumpBack, 0);
    default:
        return instruction_t(OpcodeType::NoOperation, 0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool Compiler::isMergable(const instruction_t& instr) noexcept
{
    switch (instr.opcode())
    {
    case OpcodeType::PtrInc:
    case OpcodeType::PtrDec:
    case OpcodeType::MemInc:
    case OpcodeType::MemDec:
        return true;
    default:
        return false;
    }
}
