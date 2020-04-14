// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <iostream> // TODO: REMOVE

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
    // Open the text file and read it into a file line by line.
    // NOTE: If performance ever becomes an issue (doubtful with Brainfreeze programs being so small) this function can
    //       be optimized to get the file size, allocate a buffer upfront and then read everything into it.
    std::ifstream infile;
    std::stringstream sstream;
    std::string line;

    // TODO: FIXME this throws an exception with getline finding EOF solution read it all at once and avoid
    //       getline.
    //infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    infile.open(filename, std::ios_base::in | std::ios_base::binary);

    while (std::getline(infile, line, '\n'))
    {
        sstream << line;
    }

    std::string code = sstream.str();

    // Compile the code into an instruction stream, and then return an interpreter with the instruction stream loaded.
    Compiler compiler;
    return std::make_unique<Interpreter>(compiler.compile(code));
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

    begin = begin;
    end = end;

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
