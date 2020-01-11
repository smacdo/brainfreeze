// Copyright 2009-2020, Scott MacDonald.
#pragma once

#include <stdint.h>
#include <vector>
#include <string>

namespace Brainfreeze
{
    constexpr char* Version = "0.2";

    struct instruction_t;
    class BFProgram;

    typedef int8_t BlockT;
    typedef int8_t OpcodeT;
    typedef std::vector<instruction_t> Instructions;        // TODO: Get rid of all these
    typedef std::vector<BlockT>      Memory;

    typedef Instructions::iterator       InstrItr;
    typedef Instructions::const_iterator InstrConstItr;
    typedef Memory::iterator             MemoryItr;
    typedef Memory::const_iterator       MemoryConstItr;

    /** Defines what operation an instruction should perform. */
    enum class OpcodeType : uint8_t
    {
        EndOfStream = 0,
        NoOperation = 1,
        PtrInc = 2,
        PtrDec = 3,
        MemInc = 4,
        MemDec = 5,
        Read = 6,
        Write = 7,
        JumpForward = 9,
        JumpBack = 10
    };

    class BFProgram
    {
    public:
        BFProgram(const std::string& code);

        // Compiles the program into VM instructions
        bool compile();

        // Runs the program
        bool run();

        // Retrieves the value stored at the memory offset
        BlockT valueAt(std::size_t offset) const;

        // Retrieves the current position of the instruction pointer
        std::size_t instructionOffset() const;

        // Retrieves the current position of the memory pointer
        std::size_t memoryPointerOffset() const;

        // Checks if the program has any errors
        bool hasErrors() const { return m_bHasErrors; }

        // Returns error text
        std::string errorText() const { return m_errorText; }

        // Raises a fatal error
        void raiseError(const std::string& errorText);

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

    /** Defines a brainfreeze instruction to be performed. */
    struct instruction_t
    {
    public:
        /** Default constructor. */
        instruction_t() noexcept;

        /** Constructor that takes an opcode. */
        explicit instruction_t(OpcodeType op) noexcept;

        /** Constructor that takes an opcode and optional argument. */
        instruction_t(OpcodeType op, int16_t arg) noexcept;

        /** Get the opcode encoded in this instruction. */
        OpcodeType opcode() const noexcept;

        /** Get the parameter value encoded in this instruction. */
        int16_t param() const noexcept;

        /** Set the parameter value encoded in this instruction. */
        void setParam(int16_t p) noexcept;

        /** Check if this instruction has an opcode of the given type. */
        bool isA(OpcodeType op) const noexcept;

    private:
        uint32_t data_ = 0;
    };

    // TODO: A bunch of these should be moved into an internal header since they are private helpers.

    /**
     * Slurps a text file from disk into memory and stores it in the provided STL string instance.
     *
     * \param filename Path to the file that will be read
     * \param filedata String instance that will be filled with the contents of
     *                 the input file
     * \return         True if the function was able to read the file in from disk,
     *                 otherwise it will return false
     */
    bool loadFromDisk(const std::string& filepath, std::string& output);

    // Checks if a character is a valid BF instruction.
    bool isInstruction(char c);

    // Takes a character and returns the equivilant Brainfreeze VM instruction
    instruction_t convert(char c);

    // Writes output
    void write(const BlockT& d);

    // Reads input
    BlockT read();
}
