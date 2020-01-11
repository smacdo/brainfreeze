// Copyright 2009-2020, Scott MacDonald.
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

#include "bf.h"

const char CHR_EOF     = '\0';
const char CHR_PTR_INC = '>';
const char CHR_PTR_DEC = '<';
const char CHR_MEM_INC = '+';
const char CHR_MEM_DEC = '-';
const char CHR_WRITE   = '.';
const char CHR_READ    = ',';
const char CHR_JMP_FWD = '[';
const char CHR_JMP_BAC = ']';

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
bool Brainfreeze::loadFromDisk(const std::string& filename, std::string& filedata)
{
    std::ifstream infile;
    std::string line;
    std::stringstream sstream;

    // Try to open the file
    infile.open(filename.c_str());

    if (infile.is_open() == false || infile.bad())
    {
        perror("Failed to open file");
        return false;
    }

    // gobble the contents of the file up
    while (std::getline(infile, line, '\n'))
    {
        sstream << line;
    }

    infile.close();

    // all done woot
    filedata = sstream.str();
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool Brainfreeze::isInstruction( char c )
{
    return c == CHR_PTR_INC || 
           c == CHR_PTR_DEC || 
           c == CHR_MEM_INC || 
           c == CHR_MEM_DEC ||
           c == CHR_WRITE   || 
           c == CHR_READ    || 
           c == CHR_JMP_FWD || 
           c == CHR_JMP_BAC;
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t Brainfreeze::convert(char c)
{
    switch (c)
    {
    case CHR_PTR_INC:
        return instruction_t(OpcodeType::PtrInc, 1);
    case CHR_PTR_DEC:
        return instruction_t(OpcodeType::PtrDec, 1);
    case CHR_MEM_INC:
        return instruction_t(OpcodeType::MemInc, 1);
    case CHR_MEM_DEC:
        return instruction_t(OpcodeType::MemDec, 1);
    case CHR_READ:
        return instruction_t(OpcodeType::Read, 0);
    case CHR_WRITE:
        return instruction_t(OpcodeType::Write, 0);
    case CHR_JMP_FWD:
        return instruction_t(OpcodeType::JumpForward, 0);
    case CHR_JMP_BAC:
        return instruction_t(OpcodeType::JumpBack, 0);
    default:
        return instruction_t(OpcodeType::NoOperation, 0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void Brainfreeze::write(const BlockT& d)
{
    std::cout << static_cast<char>(d);
}

//---------------------------------------------------------------------------------------------------------------------
BlockT Brainfreeze::read()
{
    BlockT d;

    if (std::cin >> d)
    {
        return d;
    }
    else    // if ( cin.eof() ) ... else ...
    {
        assert("Invalid input failure");
    }

    return d;
}
