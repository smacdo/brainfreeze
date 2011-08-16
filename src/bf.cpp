/**
 * Brainfreeze Language Interpreter
 * (c) 2009 Scott MacDonald. All rights reserved.
 *
 * Virtual machine source code
 */
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cassert>

#include "bf.h"

BFProgram::BFProgram( const std::string& codestr )
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

bool BFProgram::run()
{
    // Make sure the program was compiled, and if it wasn't then compile it
    if (! m_bCompiled )
    {
        compile();
    }

    // Obviously we shouldn't be running if there were errors
    if ( hasErrors() )
    {
        return false;
    }

    // Run the application until we hit the terminating point
    //   XXX can we get rid of the null sentinel?
    while( m_ip != m_instructions.end() && !m_ip->isA(OP_EOF) )
    {
        runStep();      // maybe bring m_ip++ here and change runStep name
    }

    return true;
}

void BFProgram::runStep()
{
    switch( m_ip->opcode() )
    {
        case OP_PTR_INC:
            assert( m_mp != m_memory.end() );
            m_mp += m_ip->param();
            break;

        case OP_PTR_DEC:
            assert( m_mp != m_memory.begin() );
            m_mp -= m_ip->param();
            break;

        case OP_MEM_INC:
            assert( m_mp != m_memory.end() );
            *m_mp += m_ip->param();
            break;

        case OP_MEM_DEC:
            assert( m_mp != m_memory.end() );
            *m_mp -= m_ip->param();
            break;

        case OP_WRITE:
            BF::write( *m_mp );
            break;

        case OP_READ:
            *m_mp = BF::read();
            break;

        case OP_JMP_FWD:
            // Only execute if byte at data pointer is zero
            if( *m_mp != 0 ) 
            {
                break;
            }
            else
            {
                m_ip += m_ip->param();
            }

            break;

        case OP_JMP_BAC:
            // Only execute if byte at data pointer is non-zero
            if( *m_mp == 0 ) 
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
 * brainfreeze instructions.
 *
 * Additionally, it performs jump optimizations so jumps no
 * longer need to be calculated on execution.
 */
bool BFProgram::compile()
{
    Instructions temp;
    Instruction  last = Instruction( OP_EOF, 0 );
    std::stack<int> jumps;  // record positions for [

    //
    // Scan the code string. Replace each BF instruction 
    // with its symbolic equivilant.
    //
    int icount = 0;
    for( std::string::iterator itr  = m_codestr.begin();
                               itr != m_codestr.end();
                             ++itr )
    {
        if( BF::isInstruction( *itr ) )
        {
            // Convert the character into an instruction
            Instruction instr = BF::convert( *itr );

            //
            // Is this a jump instruction?
            //
            if ( instr.opcode() == OP_JMP_FWD )
            {
                // This is a forward jump. Record its position
                jumps.push(icount);
            }
            else if ( instr.opcode() == OP_JMP_BAC )
            {
                // This is a backward jump. Pop the corresponding
                // forward jump marker off the stack, and update both
                // of the instructions with the location of their 
                // corresponding targets.
                int backpos = jumps.top(); jumps.pop();
                int dist    = icount - backpos;

                assert( dist > 0 );

                // Update the [ character
                temp[backpos].setParam( dist );

                // Update the ] (current) character
                instr.setParam( dist );
            }

            //
            // Was the last character a repeat of +, -, >, <
            //
            if ( ( instr.opcode() == last.opcode() ) &&
                 ( instr.opcode() == OP_PTR_INC || 
                   instr.opcode() == OP_PTR_DEC ||
                   instr.opcode() == OP_MEM_INC ||
                   instr.opcode() == OP_MEM_DEC ) )
            {
                // Get the last character, and update its occurrence
                temp[icount-1].setParam( temp[icount-1].param() + 1 );
            }
            else
            {
                // Add the instruction to the instruction stream
                temp.push_back( instr );

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
    if (! jumps.empty() )
    {
        raiseError( "Mismatched jump detected when compiling" );
        return false;
    }

    // Insert end of program instruction
    temp.push_back( Instruction( OP_EOF, 0 ) );
    
    // Save it to m_instructions
    m_instructions.swap( temp );
    m_ip = m_instructions.begin();

    return true;
}

BlockT BFProgram::valueAt( std::size_t offset ) const
{
    assert( offset < m_memory.size() );

    return m_memory[ offset ];
}

std::size_t BFProgram::instructionOffset() const
{
    return m_ip - m_instructions.begin();
}

std::size_t BFProgram::memoryPointerOffset() const
{
    return m_mp - m_memory.begin();
}

/**
 * Raises an error with the program, and halts execution
 */
void BFProgram::raiseError( const std::string& message )
{
    m_bHasErrors = true;

    // Should we set the message text, or append to an already existing
    // one?
    if ( message.empty() )
    {
        m_errorText = message;
    }
    else
    {
        m_errorText += "\n";
        m_errorText += message;
    }
}
