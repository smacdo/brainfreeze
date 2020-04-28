// Copyright 2009-2020, Scott MacDonald.
#pragma once

#include "instruction.h"
#include "compiler.h"
#include "iconsole.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace Brainfreeze
{
    constexpr const char* Version = "0.2";

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
            std::unique_ptr<IConsole> console);

        /** Destructor. */
        ~Interpreter();

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
        IConsole* console() const { return console_.get(); }

        /** Set the console used by the interpreter. */
        void setConsole(std::unique_ptr<IConsole> console) { console_ = std::move(console); }

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

        std::unique_ptr<IConsole> console_;
    };
}
