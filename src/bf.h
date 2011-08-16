/**
 * Brainfreeze Language Interpreter
 * (c) 2009 Scott MacDonald. All rights reserved.
 *
 * Global header file
 */
#ifndef BRAINFREEZE_H
#define BRAINFREEZE_H

#define BRAINFREEZE_VERSION 0.2

#include <stdint.h>
#include <vector>
#include <string>

class Instruction;
class BFProgram;

typedef int8_t BlockT;
typedef int8_t OpcodeT;
typedef std::vector<Instruction> Instructions;
typedef std::vector<BlockT>      Memory;

typedef Instructions::iterator       InstrItr;
typedef Instructions::const_iterator InstrConstItr;
typedef Memory::iterator             MemoryItr;
typedef Memory::const_iterator       MemoryConstItr;

enum Opcodes
{
    OP_EOF = 0,
    OP_NOP = 1,
    OP_PTR_INC = 2,
    OP_PTR_DEC = 3,
    OP_MEM_INC = 4,
    OP_MEM_DEC = 5,
    OP_READ    = 6,
    OP_WRITE   = 7,
    OP_JMP_FWD = 9,
    OP_JMP_BAC = 10
};

void runTests();

namespace BF
{
    // Slurps file into a STL string
    bool loadFromDisk( const std::string& filepath, std::string& output );

    // Checks if a character is a valid BF instruction.
    bool isInstruction( char c );

    // Takes a character and returns the equivilant Brainfreeze VM instruction
    Instruction convert( char c );

    // Writes output
    void write( const BlockT& d );

    // Reads input
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
