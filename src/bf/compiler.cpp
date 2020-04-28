// Copyright 2009-2020, Scott MacDonald.
#include "bf/compiler.h"
#include "bf/helpers.h"
#include "bf/bf.h"
#include "bf/exceptions.h"

#include <stack>
#include <cassert>
#include <limits>
#include <stdexcept>

using namespace Brainfreeze;
using namespace Brainfreeze::Helpers;

//---------------------------------------------------------------------------------------------------------------------
std::vector<instruction_t> Compiler::compile(std::string_view programtext) const
{
    std::vector<instruction_t> instructions;
    instructions.reserve(programtext.size());      // Over-reserve for speed, and release extra at the end of compile.

    // Track jump targets for optimization (And also report when unbalanced jumps are encountered).
    std::stack<size_t> jumps;

    // Track source code position for error reporting.
    size_t nextCharIndex = 0;
    int lineNumber = 1;
    int columnNumber = 0;

    // Convert each legal character in the programtext to its compiled brainfreeze instruction form.    
    for (auto c : programtext)
    {
        // Track source code position.
        nextCharIndex++;
        lineNumber = (c == '\n' ? lineNumber + 1 : lineNumber);
        columnNumber = (c != '\n' ? columnNumber + 1 : 0);

        // Skip characters that are not legal brainfreeze instructions.
        if (!IsInstruction(c))
        {
            continue;
        }

        // Calculate the index that will be used for the next instruction written into the program.
        auto nextIndex = instructions.size();

        // Get the instruction form for this character.
        auto instr = AsInstruction(c);

        // Handle jump instructions specially. Jump forwards need to have their position recorded so that when the
        // matching backward jump is found both jumps can have their offset written into them for optimization.
        if (instr.isA(OpcodeType::JumpForward))
        {
            // If jump optimization is enabled upgrade this instruction to a fast jump forward.
            if (isPrecalculateJumpOffsetsEnabled())
            {
                instr.setOpcode(OpcodeType::FastJumpForward);
            }

            // Record the jump forward position until the matching jumping backward instruction is located.
            jumps.push(nextIndex);
        }
        else if (instr.isA(OpcodeType::JumpBack))
        {
            // Backward jump found. Pop the top jump marker off the stack which is the matching forwrad jump target.
            // Write the offset of the jump into both the forward and backward jump instructions.

            // Throw an exception if there is not a matching forward [ jump.
            if (jumps.empty())
            {
                throw CompileException(
                    "Unbalanced jump, expected a [ before this ]",
                    nextCharIndex - 1,
                    lineNumber,
                    columnNumber);
            }

            // If the jump optimization is enabled upgrade this instruction to a fast jump backward. Search for the
            // corresponding forward jump instruction and write the offset into both. If the optimization is disabled
            // then leave the jump instruction alone.
            if (isPrecalculateJumpOffsetsEnabled())
            {
                // Upgrade to fast jump.
                instr.setOpcode(OpcodeType::FastJumpBack);

                // Get the offset of the matching forward [ jump.
                auto forwardJumpOffset = jumps.top();
                jumps.pop();

                auto distance = nextIndex - forwardJumpOffset;
                assert(distance > 0);

                // Verify the jump distance is small enough to fit in the instruction parameter.
                // TODO: This should be supported on the off-chance it is encountered in the real world.
                if (distance > (size_t)std::numeric_limits<instruction_t::param_t>::max())
                {
                    throw CompileException(
                        "Jump target to large to fit in instruction",
                        nextCharIndex - 1,
                        lineNumber,
                        columnNumber);
                }

                // Store the offset in the [ instruction.
                instructions[forwardJumpOffset].setParam(static_cast<instruction_t::param_t>(distance));

                // Store the offset in the current ] instruction.
                instr.setParam(static_cast<instruction_t::param_t>(distance));
            }
            else
            {
                // Make sure the corresponding forward jump offset is removed from the stack even if the optimiation
                // isn't being used. (The stack is still tracking if there are unbalanced jumps).
                jumps.pop();
            }
        }

        // Is this instruction a repeat of the previous instruction? If it is a repeating instruction that supports
        // merging (like increment/decrement) then merge it into the last instruction and increase the parameter
        // count.
        if (mergeInstructions_ &&                                   // is the merge instruction optimization enabled?
            isMergable(instr) &&                                    // is this instruction mergable?
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
        throw CompileException(
            "Unbalanced jump, expected a ] before program termination",
            nextCharIndex - 1,
            lineNumber,
            columnNumber);
    }

    // Insert end of program instruction.
    instructions.push_back(instruction_t(OpcodeType::EndOfStream));

    // Remove unused space from the list of instructions before returning.
    instructions.shrink_to_fit();
    return instructions;
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
