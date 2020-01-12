// Copyright 2009-2020, Scott MacDonald.
#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace Brainfreeze
{
    constexpr char* Version = "0.2";
    struct instruction_t;

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

    /** The brainfreeze interpreter. */
    class Interpreter
    {
    public:
        using byte_t = int8_t;

        enum class RunState
        {
            NotStarted,
            Running,
            Finished
        };

    public:
        /** Construct interpreter with code to be run. */
        Interpreter(std::vector<instruction_t> instructions);

    public:
        /** Get the number of memory cells to allocate for execution. */
        std::size_t cellCount() const noexcept { return cellCount_; }

        /** Set the number of memory cells to allocate for execution. */
        void setCellCount(size_t count);

        /** Get the size in bytes for a memory cell. */
        std::size_t cellSize() const noexcept { return cellSize_; }

        /** Set the size in bytes for a memory cell. */
        void setCellSize(size_t bytes);

        /** Get the read function used by the interpreter. */
        std::function<byte_t(void)> readFunction() const { return readFunction_; }

        /** Set the read function used by the interpreter. */
        void setReadFunction(std::function<byte_t(void)> func) { readFunction_ = std::move(func); }

        /** Get the write function used by the interprter. */
        std::function<void(byte_t)> writeFunction() const { return writeFunction_; }

        /** Set the write function used by the interpreter. */
        void setWriteFunction(std::function<void(byte_t)> func) { writeFunction_ = std::move(func); }

    public:
        /** Execute the Brainfreeze program and do not return until execution has finished. */
        void run();

        /**
         * Get the value stored at the requested memory address.
         *
         * \param   The memory offset to fetch
         * \returns The value that was stored in that memory block
         */
        byte_t memoryAt(std::size_t address) const;

        /** Get the current instruction pointer. */
        std::size_t instructionOffset() const;

        /** Get the current memory pointer. */
        std::size_t memoryPointerOffset() const;

    private:
        /** Prepares the interpreter before execution begins. */
        void start();

        /** Execute the next instruction and return the running state after executing the one step. */
        RunState runStep();

    private:
        std::vector<instruction_t> instructions_;
        std::vector<byte_t> memory_;

        // TODO: Make these be size_t offsets
        std::vector<instruction_t>::const_iterator ip_;

        // Current memory offset
        std::vector<byte_t>::iterator mp_;

        RunState state_ = RunState::NotStarted;

        std::size_t cellCount_ = 30000;
        std::size_t cellSize_ = 1;

        std::function<void(byte_t)> writeFunction_;
        std::function<byte_t(void)> readFunction_;
    };

    /** Compiles Brainfreeze code into executable instructions. */
    class Compiler
    {
    public:
        /** Convert Brainfreeze code into an exeuctable Brainfreeze program. */
        std::vector<instruction_t> compile(std::string_view programtext) const;

        /** Get if a character is a valid brainfreeze instruction. */
        static bool isInstruction(char c) noexcept;

        /** Get the compiled instruction form of a brainfreeze character. */
        static instruction_t asInstruction(char c);

        /** Get if an instruction can be merged together for optimization. TODO: move this. */
        static bool isMergable(const instruction_t& instr) noexcept;
    };

    /** Defines an executable Brainfreeze instruction. */
    struct instruction_t
    {
    public:
        using param_t = int16_t;

    public:
        /** Default constructor, defaults to a NOP instruction. */
        instruction_t() noexcept;

        /** Constructor that takes an opcode. */
        explicit instruction_t(OpcodeType op) noexcept;

        /** Constructor that takes an opcode and optional parameter value. */
        instruction_t(OpcodeType op, param_t value) noexcept;

        /** Get the opcode encoded in this instruction. */
        OpcodeType opcode() const noexcept;

        /** Check if this instruction has an opcode of the given type. */
        bool isA(OpcodeType op) const noexcept;

        /** Get the parameter value encoded in this instruction. */
        param_t param() const noexcept;

        /** Set the parameter value encoded in this instruction. */
        void setParam(param_t value) noexcept;

        /**
         * Increment parameter by the given amount.
         * An exception is thrown if the new value is too large or too small to be stored.
         */
        void incrementParam(param_t amount);

    private:
        uint32_t data_ = 0;
    };

    namespace Helpers
    {
        /**
         * Read a text file containing Brainfreeze code from disk, compile it and return an interpreter instance
         * capable of executing the code. Throws an exception if something goes wrong while trying to load or compile
         * the code.
         *
         * \param    filename Path to the file that will be read.
         * \returns  New interpreter that is ready to run the loaded code.
         */
        std::unique_ptr<Interpreter> LoadFromDisk(const std::string& filepath);
    }

    namespace Details
    {
        /** Default write implementation: writes a byte to standard output. */
        void Write(Interpreter::byte_t d);

        /** Default read implementation: reads a byte from standard input. */
        Interpreter::byte_t Read();
    }
}
