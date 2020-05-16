// Copyright 2009-2020, Scott MacDonald.
#include "bf/helpers.h"
#include "bf/bf.h"
#include "bf/exceptions.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>

using namespace Brainfreeze;

// TODO: Maybe put this in a header for other poeple to use?
namespace Characters
{
    const char END_OF_STREAM = '0';
    const char PTR_INC = '>';
    const char PTR_DEC = '<';
    const char MEM_INC = '+';
    const char MEM_DEC = '-';
    const char WRITE = '.';
    const char READ = ',';
    const char JUMP_FORWARD = '[';
    const char JUMP_BACK = ']';
    const char NOP = '~';
}

//---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<Interpreter> Brainfreeze::Helpers::LoadFromDisk(const std::string& filename)
{
    // Check that the path exists and is a file.
    if (!std::filesystem::exists(filename))
    {
        throw CompileException("Path to source code file does not exist", (size_t)-1, 0, 0);
    }
    else if (!std::filesystem::is_regular_file(filename))
    {
        throw CompileException("Path to soure code file is not a file", (size_t)-1, 0, 0);
    }

    // Reserve enough space to hold the file and then read it into memory all at once.
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    const auto size = std::filesystem::file_size(filename);
    std::string buffer(size, '\0');

    // Read the whole file into the buffer.
    stream.read(buffer.data(), size);

    // Compile the code into an instruction stream, and then return an interpreter with the instruction stream loaded.
    // TODO: Make it so the caller can pass options to the compiler.
    Compiler compiler;
    return std::make_unique<Interpreter>(compiler.compile(buffer));
}

//---------------------------------------------------------------------------------------------------------------------
std::vector<instruction_t>::const_iterator Brainfreeze::Helpers::FindJumpTarget(
    std::vector<instruction_t>::const_iterator begin,
    std::vector<instruction_t>::const_iterator end,
    std::vector<instruction_t>::const_iterator jump)
{
    assert(jump >= begin);
    assert(jump < end);
    assert(jump->isA(OpcodeType::JumpForward) || jump->isA(OpcodeType::JumpBack));

    if (jump->isA(OpcodeType::JumpForward))
    {
        int depth = 1;
        auto itr = jump;

        do
        {
            itr++;

            assert(itr >= begin);
            assert(itr < end);

            if (itr->isA(OpcodeType::JumpForward))
            {
                depth++;
            }
            else if (itr->isA(OpcodeType::JumpBack))
            {
                depth--;
            }

        } while (depth > 0);

        return itr;
    }
    else if (jump->isA(OpcodeType::JumpBack))
    {
        int depth = 1;
        auto itr = jump;

        do
        {
            itr--;

            assert(itr >= begin);
            assert(itr < end);

            if (itr->isA(OpcodeType::JumpForward))
            {
                depth--;
            }
            else if (itr->isA(OpcodeType::JumpBack))
            {
                depth++;
            }
        } while (depth > 0);

        return itr;
    }
    else
    {
        throw std::runtime_error("Not a valid jump instruction");
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool Brainfreeze::Helpers::IsInstruction(char c) noexcept
{
    return c == Characters::PTR_INC
        || c == Characters::PTR_DEC
        || c == Characters::MEM_INC
        || c == Characters::MEM_DEC
        || c == Characters::WRITE
        || c == Characters::READ
        || c == Characters::JUMP_FORWARD
        || c == Characters::JUMP_BACK;
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t Brainfreeze::Helpers::AsInstruction(char c)
{
    switch (c)
    {
    case Characters::PTR_INC:
        return instruction_t(OpcodeType::PtrInc, 1);
    case Characters::PTR_DEC:
        return instruction_t(OpcodeType::PtrDec, 1);
    case Characters::MEM_INC:
        return instruction_t(OpcodeType::MemInc, 1);
    case Characters::MEM_DEC:
        return instruction_t(OpcodeType::MemDec, 1);
    case Characters::READ:
        return instruction_t(OpcodeType::Read, 0);
    case Characters::WRITE:
        return instruction_t(OpcodeType::Write, 0);
    case Characters::JUMP_FORWARD:
        return instruction_t(OpcodeType::JumpForward, 0);
    case Characters::JUMP_BACK:
        return instruction_t(OpcodeType::JumpBack, 0);
    default:
        return instruction_t(OpcodeType::NoOperation, 0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
char Brainfreeze::Helpers::AsChar(OpcodeType opcode)
{
    switch (opcode)
    {
    case OpcodeType::EndOfStream:
        return Characters::END_OF_STREAM;
    case OpcodeType::NoOperation:
        return Characters::NOP;
    case OpcodeType::PtrInc:
        return Characters::PTR_INC;
    case OpcodeType::PtrDec:
        return Characters::PTR_DEC;
    case OpcodeType::MemInc:
        return Characters::MEM_INC;
    case OpcodeType::MemDec:
        return Characters::MEM_DEC;
    case OpcodeType::Read:
        return Characters::READ;
    case OpcodeType::Write:
        return Characters::WRITE;
    case OpcodeType::JumpForward:
    case OpcodeType::FastJumpForward:
        return Characters::JUMP_FORWARD;
    case OpcodeType::JumpBack:
    case OpcodeType::FastJumpBack:
        return Characters::JUMP_BACK;
    default:
        throw std::runtime_error("Unrecogonized opcode when converting to character");
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string Brainfreeze::Helpers::AsName(OpcodeType opcode)
{
    switch (opcode)
    {
    case OpcodeType::EndOfStream:
        return "EndOfStream";
    case OpcodeType::NoOperation:
        return "NoOperation";
    case OpcodeType::PtrInc:
        return "PtrInc";
    case OpcodeType::PtrDec:
        return "PtrDec";
    case OpcodeType::MemInc:
        return "MemInc";
    case OpcodeType::MemDec:
        return "MemDec";
    case OpcodeType::Read:
        return "Read";
    case OpcodeType::Write:
        return "Write";
    case OpcodeType::JumpForward:
        return "JumpForward";
    case OpcodeType::FastJumpForward:
        return "FastJumpForward";
    case OpcodeType::JumpBack:
        return "JumpBack";
    case OpcodeType::FastJumpBack:
        return "FastJumpBack";
    default:
        throw std::runtime_error("Unrecogonized opcode when converting to character");
    }
}
