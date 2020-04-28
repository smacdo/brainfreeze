// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "instruction.h"

#include <vector>
#include <string>

namespace Brainfreeze
{
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
}
