#pragma once
#include "bf.h"

#include <catch2/catch.hpp>
#include <string>

namespace Brainfreeze
{
    namespace TestHelpers
    {
        /** Matches memory values in a brainfreeze proram. */
        // TODO: Take a memory pointer type for better test/matching.
        class MemoryMatcher : public Catch::MatcherBase<Interpreter>
        {
        public:
            explicit MemoryMatcher(size_t offset, Interpreter::byte_t expected);
            virtual bool match(const Interpreter& program) const override;
            virtual std::string describe() const override;

        private:
            size_t offset_;
            Interpreter::byte_t expected_;
        };

        /** Matches instruction pointer in a brainfreeze proram. */
        // TODO: Take a instructon pointer type for better test/matching.
        class InstructionPointerMatcher : public Catch::MatcherBase<Interpreter>
        {
        public:
            explicit InstructionPointerMatcher(size_t expectedOffset);
            virtual bool match(const Interpreter& program) const override;
            virtual std::string describe() const override;

        private:
            size_t expectedOffset_;
        };

        /** Matches memory pointer in a brainfreeze proram. */
        // TODO: Take a memory pointer type for better test/matching.
        class MemoryPointerMatcher : public Catch::MatcherBase<Interpreter>
        {
        public:
            explicit MemoryPointerMatcher(size_t expectedOffset);
            virtual bool match(const Interpreter& program) const override;
            virtual std::string describe() const override;

        private:
            size_t expectedOffset_;
        };

        MemoryMatcher HasMemory(size_t offset, Interpreter::byte_t expected);
        InstructionPointerMatcher InstructionPointerIs(size_t expectedOffset);
        MemoryPointerMatcher MemoryPointerIs(size_t expectedOffset);
    }
}