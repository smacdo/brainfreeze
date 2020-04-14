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
        JumpBack = 10,
        FastJumpForward = 11,
        FastJumpBack = 12
    };

    class Console;

    /** The brainfreeze interpreter. */
    class Interpreter
    {
    public:
        using byte_t = int8_t;
        using instruction_list_t = std::vector<instruction_t>;
        using memory_buffer_t = std::vector<byte_t>;

        /** Opaque instruction pointer type. */
        struct instruction_pointer_t
        {
            explicit instruction_pointer_t(
                    instruction_list_t::const_iterator begin,
                    instruction_list_t::const_iterator current)
                : begin_(begin), current_(current)
            {
            }

            bool operator ==(const instruction_pointer_t& other) const
            {
                return begin_ == other.begin_ && current_ == other.current_;
            }

            std::size_t address() const
            {
                return current_ - begin_;
            }
            
            instruction_list_t::const_iterator begin_;
            instruction_list_t::const_iterator current_;
        };

        /** Opaque memory pointer type. */
        struct memory_pointer_t
        {
            explicit memory_pointer_t(
                    memory_buffer_t::const_iterator begin,
                    memory_buffer_t::const_iterator current)
                : begin_(begin), current_(current)
            {
            }

            bool operator ==(const memory_pointer_t& other) const
            {
                return begin_ == other.begin_ && current_ == other.current_;
            }

            std::size_t address() const
            {
                return current_ - begin_;
            }

            byte_t data() const
            {
                return *current_;
            }

            memory_buffer_t::const_iterator begin_;
            memory_buffer_t::const_iterator current_;
        };

        enum class RunState
        {
            NotStarted,
            Running,
            Finished
        };

        enum class EndOfStreamBehavior
        {
            Zero = 0,
            NegativeOne = 1,
            NoChange = 2,
            Ignore = 3
        };

    public:
        /** Construct interpreter with code to be run. */
        Interpreter(std::vector<instruction_t> instructions);

        /** Construct interpreter with code to be run. */
        Interpreter(
            std::vector<instruction_t> instructions,
            std::unique_ptr<Console> console);

    public:
        /** Get the number of memory cells to allocate for execution. */
        std::size_t cellCount() const noexcept { return cellCount_; }

        /** Set the number of memory cells to allocate for execution. */
        void setCellCount(size_t count);

        /** Get the size in bytes for a memory cell. */
        std::size_t cellSize() const noexcept { return cellSize_; }

        /** Set the size in bytes for a memory cell. */
        void setCellSize(size_t bytes);

        /** Get the end of stream behavior. */
        EndOfStreamBehavior endOfStreamBehavior() const noexcept { return endOfStreamBehavior_; }

        /** Set the end of stream behavior. */
        void setEndOfStreamBehavior(EndOfStreamBehavior behavior) noexcept { endOfStreamBehavior_ = behavior; }

        /** Get the console used by the interpreter. */
        Console* console() const { return console_.get(); }

        /** Set the console used by the interpreter. */
        void setConsole(std::unique_ptr<Console> console) { console_ = std::move(console); }

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
        instruction_pointer_t instructionPointer() const;

        /** Get the current memory pointer. */
        memory_pointer_t memoryPointer() const;

    private:
        /** Prepares the interpreter before execution begins. */
        void start();

        /** Execute the next instruction and return the running state after executing the one step. */
        RunState runStep();

    private:
        instruction_list_t instructions_;
        memory_buffer_t memory_;

        instruction_list_t::const_iterator ip_;
        memory_buffer_t::iterator mp_;

        RunState state_ = RunState::NotStarted;

        std::size_t cellCount_ = 30000;
        std::size_t cellSize_ = 1;
        EndOfStreamBehavior endOfStreamBehavior_ = EndOfStreamBehavior::NegativeOne;

        std::unique_ptr<Console> console_;
    };

    /** Compiles Brainfreeze code into executable instructions. */
    class Compiler
    {
    public:
        /** Convert Brainfreeze code into an executable Brainfreeze program. */
        std::vector<instruction_t> compile(std::string_view programtext) const;

    public:
        /** Get if the compiler can merge a sequence of identical instructions together. */
        bool isMergeInstructionsEnabled() const noexcept { return mergeInstructions_; }

        /** Set if the compiler can merge a sequence of identical instructions together. */
        void setMergeInstructionsEnabled(bool isEnabled) noexcept { mergeInstructions_ = isEnabled; }

        /** Get if the compiler can precalculate the distance to the corresponding jump target. */
        bool isPrecalculateJumpOffsetsEnabled() const noexcept { return precalculateJumpOffsets_; }

        /** Set if the compiler can precalculate the distance to the corresponding jump target. */
        void setPrecalculateJumpOffsetsEnabled(bool isEnabled) noexcept { precalculateJumpOffsets_ = isEnabled; }

    public:
        /** Get if an instruction can be merged together for optimization. TODO: move this. */
        static bool isMergable(const instruction_t& instr) noexcept;

    private:
        bool mergeInstructions_ = true;
        bool precalculateJumpOffsets_ = true;
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

        /** Set the opcode encoded in this instruction. */
        void setOpcode(OpcodeType op) noexcept;

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

        /** Equality comparison operator. */
        bool operator ==(const instruction_t& other) const noexcept;

        /** Inequality comparison operator. */
        bool operator !=(const instruction_t& other) const noexcept;

    private:
        uint32_t data_ = 0;
    };

    /** Abstracts console window handling from the interpreter. */
    class Console
    {
    public:
        /** Destructor. */
        virtual ~Console();

        /** Get if characters should be echoed when typed. */
        bool shouldEchoCharForInput() const noexcept { return bEchoCharWhenReading_; }

        /** Set if characters should be echoed when typed. */
        void setShouldEchoCharForInput(bool newValue) noexcept { bEchoCharWhenReading_ = newValue; }

        /** Get if CR (\r and \r\n) should be converted to LF (\n) when read from input. */
        bool shouldConvertInputCRtoLF() const noexcept { return bConvertInputCRtoLF_; }

        /** Set if CR (\r and \r\n) should be converted to LF (\n) when read from input. */
        void setShouldConvertInputCRtoLF(bool newValue) noexcept { bConvertInputCRtoLF_ = newValue; }

        /** Get if LF (\n) should be converted to CRLF (\r\n) when written to output. */
        bool shouldConvertOutputLFtoCRLF() const noexcept { return bConvertOutputLFtoCRLF_; }

        /** Set if LF (\n) should be converted to CRLF (\r\n) when written to output. */
        void setShouldConvertOutputLFtoCRLF(bool newValue) noexcept { bConvertOutputLFtoCRLF_ = newValue; }

        /** Default write implementation: writes a byte to standard output. */
        virtual void Write(char d) = 0;

        /** Default read implementation: reads a byte from standard input. */
        virtual char Read() = 0;

    private:
        bool bEchoCharWhenReading_ = true;
        bool bConvertInputCRtoLF_ = true;
        bool bConvertOutputLFtoCRLF_ = true;
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

        /**
         * Find the location of the matching jump instruction for a given jump in the Brainfreeze program.
         * ex: Given a program "+[[-]]", FindJumpTarget(1) would return 5.
         * NOTE: This function has undefined behavior if there is not a matching jump instruction!
         *       (to preserve runtime speed).
         */
        std::vector<instruction_t>::const_iterator FindJumpTarget(
            std::vector<instruction_t>::const_iterator begin,
            std::vector<instruction_t>::const_iterator end,
            std::vector<instruction_t>::const_iterator jump);

        /** Get if a character is a valid brainfreeze instruction. */
        bool IsInstruction(char c) noexcept;

        /** Get the compiled instruction form of a brainfreeze character. */
        instruction_t AsInstruction(char c);

        /** Get the character corresponding to a Brainfreeze instruction. */
        char AsChar(OpcodeType instruction);

        /** Get the name of a Brainfreeze instruction. */
        std::string AsName(OpcodeType instruction);
    }
}
