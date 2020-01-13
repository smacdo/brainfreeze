#pragma once
#include "bf.h"

#include <catch2/catch.hpp>
#include <string>
#include <ostream>

namespace Brainfreeze
{
    namespace TestHelpers
    {
        std::vector<instruction_t> Compile(
            const std::string& code,
            std::function<void(Compiler&)>&& configureCallback);
        std::vector<instruction_t> Compile(const std::string& code);
        Interpreter CreateInterpreter(const std::string& code);
        
        /** Matches memory values in a brainfreeze proram. */
        // TODO: Take a memory pointer type for better test/matching.
        class MemoryMatcher : public Catch::MatcherBase<Interpreter>
        {
        public:
            explicit MemoryMatcher(size_t address, Interpreter::byte_t expected);
            virtual bool match(const Interpreter& program) const override;
            virtual std::string describe() const override;

        private:
            size_t address_;
            Interpreter::byte_t expected_;
        };

        /** Matches instruction pointer in a brainfreeze proram. */
        class InstructionPointerMatcher : public Catch::MatcherBase<Interpreter::instruction_pointer_t>
        {
        public:
            explicit InstructionPointerMatcher(std::size_t expectedAddress);
            virtual bool match(const Interpreter::instruction_pointer_t& actualIP) const override;
            virtual std::string describe() const override;

        private:
            std::size_t expectedAddress_ = 0;
        };

        /** Matches memory pointer in a brainfreeze proram. */
        class MemoryPointerMatcher : public Catch::MatcherBase<Interpreter::memory_pointer_t>
        {
        public:
            explicit MemoryPointerMatcher(std::size_t expectedAddress);
            virtual bool match(const Interpreter::memory_pointer_t& actualMP) const override;
            virtual std::string describe() const override;

        private:
            std::size_t expectedAddress_ = 0;
        };

        MemoryMatcher HasMemory(size_t offset, Interpreter::byte_t expected);
        InstructionPointerMatcher InstructionPointerIs(std::size_t expectedAddress);  // TODO: MemoryAdddressIs
        MemoryPointerMatcher MemoryPointerIs(std::size_t expectedAddress);
    }

    std::ostream& operator <<(std::ostream& os, Interpreter::instruction_pointer_t ip);
    std::ostream& operator <<(std::ostream& os, Interpreter::memory_pointer_t mp);
    std::ostream& operator <<(std::ostream& os, instruction_t instruction);
    std::ostream& operator <<(std::ostream& os, std::vector<instruction_t>::const_iterator itr);
}
