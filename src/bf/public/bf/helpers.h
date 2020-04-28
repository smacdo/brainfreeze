// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "instruction.h"

#include <vector>
#include <memory>
#include <string>

namespace Brainfreeze
{
    class Interpreter;
}

namespace Brainfreeze::Helpers
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
