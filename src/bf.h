/**
 * Brainfreeze Language Interpreter
 * (c) 2009 Scott MacDonald. All rights reserved.
 *
 * Global header file
 */
#ifndef BRAINFREEZE_H
#define BRAINFREEZE_H

#include <stdint.h>
#include <vector>
#include <string>

class Instruction;
class BFProgram;

typedef int8_t BlockT;
typedef std::vector<Instruction> Instructions;
typedef std::vector<BlockT>      Memory;

typedef Instructions::iterator       InstrItr;
typedef Instructions::const_iterator InstrConstItr;
typedef Memory::iterator             MemoryItr;
typedef Memory::const_iterator       MemoryConstItr;

void runTests();

namespace BF
{
    bool isInstruction( char c );

    Instruction convert( char c );

    void write( const BlockT& d );

    BlockT read();
}

class BFProgram
{
public:
    BFProgram( const std::string& code );

    // Compiles the program into VM instructions
    bool compile();

    // Runs the program
    bool run();

    // Retrieves the value stored at the memory offset
    BlockT valueAt( std::size_t offset ) const;

    // Retrieves the current position of the instruction pointer
    std::size_t instructionOffset() const;

    // Retrieves the current position of the memory pointer
    std::size_t memoryPointerOffset() const;

    // Checks if the program has any errors
    bool hasErrors() const { return m_bHasErrors; }

    // Returns error text
    std::string errorText() const { return m_errorText; }

    // Raises a fatal error
    void raiseError( const std::string& errorText );

private:
    // Executes the next instruction in the instruction stream
    void runStep();

    // The program's source code
    std::string  m_codestr;

    // The transformed list of VM instructions
    Instructions m_instructions;

    // The program's memory
    Memory       m_memory;

    // Current instruction offset
    InstrItr     m_ip;

    // Current memory offset
    MemoryItr    m_mp;

    // Flag checking if code was compiled to VM bytecode
    bool m_bCompiled;
    
    // Flag checking if execution has finished
    bool m_bFinished;

    // Flag specifying if there were fatal errors
    bool m_bHasErrors;

    // Contains error text
    std::string m_errorText;
};

class Instruction
{
public:
    Instruction( uint8_t op, uint32_t arg )
        : m_idata( ( 0x000000FF & op ) | ( arg << 8 ) )
    {
    }

    uint8_t opcode() const
    {
        return m_idata & 0x000000FF;
    }

    uint32_t param() const
    {
        return (m_idata & 0xFFFFFF00) >> 8;
    }

    void setParam( uint32_t p )
    {
        // Clear the param
        m_idata = 0x000000FF & m_idata;

        // Set the param
        m_idata = ( p << 8 ) | m_idata;
    }
	
	bool isA( uint8_t op ) const
	{
		return ( (m_idata & 0x000000FF) == op );
	}

private:
    uint32_t m_idata;
};

#endif
