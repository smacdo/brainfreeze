// Copyright 2009-2020, Scott MacDonald.
#include <stack>
#include <string>
#include <cassert>

#include "bf.h"

using namespace Brainfreeze;

/**
 * Brainfreeze program constructor. Takes as input the brainfreeze program
 * that it will eventually run
 *
 * \param codestr The brainfreeze program
 */
BFProgram::BFProgram(const std::string& codestr)
    : m_codestr(codestr),
    m_instructions(),
    m_memory(),
    m_ip(),
    m_mp(),
    m_bCompiled(false),
    m_bFinished(false),
    m_bHasErrors(false)
{
    m_instructions.clear();
    m_memory.resize(1024 * 32);

    m_mp = m_memory.begin();
}

/**
 * Run the loaded brainfreeze program. This method will run the brainfreeze
 * program until it has reached the end of script command.
 *
 * \returns True if the program executed through to completion, or false if
 *          there were errors when attempting to run.
 */
bool BFProgram::run()
{
    // Make sure the program was compiled, and if it wasn't then compile it
    if (!m_bCompiled)
    {
        compile();
    }

    // Obviously we shouldn't be running if there were errors
    if (hasErrors())
    {
        return false;
    }

    // Run the application until we hit the terminating point
    //   XXX can we get rid of the null sentinel?
    while (m_ip != m_instructions.end() && !m_ip->isA(OpcodeType::EndOfStream))
    {
        runStep();      // maybe bring m_ip++ here and change runStep name
    }

    return true;
}

/**
 * Runs the next instruction that is scheduled for execution.
 */
void BFProgram::runStep()
{
    switch (m_ip->opcode())
    {
    case OpcodeType::PtrInc:
        assert(m_mp != m_memory.end());
        m_mp += m_ip->param();
        break;

    case OpcodeType::PtrDec:
        assert(m_mp != m_memory.begin());
        m_mp -= m_ip->param();
        break;

    case OpcodeType::MemInc:
        assert(m_mp != m_memory.end());
        // TODO: Handle configurable memory blocks larger than 1 byte.
        // TODO: How should overflow be handled?
        *m_mp += static_cast<BlockT>(m_ip->param());
        break;

    case OpcodeType::MemDec:
        assert(m_mp != m_memory.end());
        // TODO: Handle configurable memory blocks larger than 1 byte.
        // TODO: How should overflow be handled?
        *m_mp -= static_cast<BlockT>(m_ip->param());
        break;

    case OpcodeType::Write:
        write(*m_mp);
        break;

    case OpcodeType::Read:
        *m_mp = read();
        break;

    case OpcodeType::JumpForward:
        // Only execute if byte at data pointer is zero
        if (*m_mp != 0)
        {
            break;
        }
        else
        {
            m_ip += m_ip->param();
        }

        break;

    case OpcodeType::JumpBack:
        // Only execute if byte at data pointer is non-zero
        if (*m_mp == 0)
        {
            break;
        }
        else
        {
            m_ip -= m_ip->param();
        }
        break;

    default:
        assert("Unknown opcode");
    }

    m_ip++;            // maybe integrate into switch
}

/**
 * Compiles a brainfreeze program. It converts the program's
 * textual representation into a program containing only the
 * brainfreeze vm instructions.
 *
 * Additionally, it performs several compile time optimizations in order to
 * speed up execution time. These optimizations include pre-calculating
 * jump offsets and fusing sequential identical instructions.
 *
 * \return Returns true if the program was succesfully compiled, otherwise
 *         it will return false to indicate the presence of errors
 */
bool BFProgram::compile()
{
    Instructions temp;
    instruction_t  last = instruction_t(OpcodeType::EndOfStream, 0);
    std::stack<size_t> jumps;  // record positions for [

    //
    // Scan the code string. Replace each BF instruction 
    // with its symbolic equivilant.
    //
    size_t icount = 0;
    for (std::string::iterator itr = m_codestr.begin();
        itr != m_codestr.end();
        ++itr)
    {
        if (isInstruction(*itr))
        {
            // Convert the character into an instruction
            instruction_t instr = convert(*itr);

            //
            // Is this a jump instruction?
            //
            if (instr.opcode() == OpcodeType::JumpForward)
            {
                // This is a forward jump. Record its position
                jumps.push(icount);
            }
            else if (instr.opcode() == OpcodeType::JumpBack)
            {
                // This is a backward jump. Pop the corresponding
                // forward jump marker off the stack, and update both
                // of the instructions with the location of their 
                // corresponding targets.
                auto backpos = jumps.top();
                jumps.pop();
                
                auto dist = icount - backpos;

                assert(dist > 0);

                // Update the [ character
                temp[backpos].setParam(static_cast<uint16_t>(dist));

                // Update the ] (current) character
                instr.setParam(static_cast<uint16_t>(dist));
            }

            //
            // Was the last character a repeat of +, -, >, <
            //
            if ((instr.opcode() == last.opcode()) &&
                (instr.opcode() == OpcodeType::PtrInc ||
                    instr.opcode() == OpcodeType::PtrDec ||
                    instr.opcode() == OpcodeType::MemInc ||
                    instr.opcode() == OpcodeType::MemDec))
            {
                // Get the last character, and update its occurrence.
                // TODO: Remove this assert and make sure the bug does not happen.
                assert(icount > 0);
                temp[icount - 1].setParam(temp[icount - 1].param() + 1);
            }
            else
            {
                // Add the instruction to the instruction stream
                temp.push_back(instr);

                // Remember last instruction
                last = instr;

                // Increment instruction counter
                icount += 1;
            }

            // Remember the last instruction
            last = instr;
        }
    }

    // Verify the jump stack is empty. If not, then there is a
    // mismatched jump somewhere!
    if (!jumps.empty())
    {
        raiseError("Mismatched jump detected when compiling");
        return false;
    }

    // Insert end of program instruction
    temp.push_back(instruction_t(OpcodeType::EndOfStream, 0));

    // Save it to m_instructions
    m_instructions.swap(temp);
    m_ip = m_instructions.begin();

    return true;
}

/**
 * Returns the specific value located at the requested memory offset
 *
 * \param   The memory offset to fetch
 * \returns The value that was stored in that memory block
 */
BlockT BFProgram::valueAt(std::size_t offset) const
{
    assert(offset < m_memory.size());

    return m_memory[offset];
}

/**
 * Returns the current instruction pointer offset
 * \returns Instruction pointer offset
 */
std::size_t BFProgram::instructionOffset() const
{
    return m_ip - m_instructions.begin();
}

/**
 * Returns the memory pointer offset
 * \returns Memory pointer offset
 */
std::size_t BFProgram::memoryPointerOffset() const
{
    return m_mp - m_memory.begin();
}

/**
 * Raises an error with this brainfreeze script, and halts execution.
 *
 * \param message A string informing the user what error occurred
 */
void BFProgram::raiseError(const std::string& message)
{
    m_bHasErrors = true;

    // Should we set the message text, or append to an already existing
    // one?
    if (message.empty())
    {
        m_errorText = message;
    }
    else
    {
        m_errorText += "\n";
        m_errorText += message;
    }
}
