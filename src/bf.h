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

typedef int8_t Data;
typedef std::vector<Instruction> Instructions;
typedef std::vector<Data>        Memory;

typedef Instructions::iterator       InstrItr;
typedef Instructions::const_iterator InstrConstItr;
typedef Memory::iterator             MemoryItr;
typedef Memory::const_iterator       MemoryConstItr;

void runTests();

namespace BF
{
    bool isInstruction( char c );

    Instruction convert( char c );

    void write( const Data& d );

    Data read();
}

class BFProgram
{
    public:
        BFProgram( const std::string& code );

        void compile();
        void run();

        /**
         * Retrieves the value stored at the specified offset in the
         * program's memory.
         *
         * \param offset The memory offset
         * \return       Value at that memory offset
         */
        Data valueAt( int offset ) const;

        /**
         * Retrieves the offset of the program's current instruction
         * pointer
         */
        std::size_t instructionOffset() const;

        /**
         * Retrieves the offset of the program's current memory pointer
         */
        std::size_t memoryPointerOffset() const;

    private:
        void runStep();

        std::string  m_codestr;
        Instructions m_instructions;
        Memory       m_memory;

        InstrItr     m_ip;
        MemoryItr    m_mp;

        bool         m_bCompiled;
        bool         m_bFinished;
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
