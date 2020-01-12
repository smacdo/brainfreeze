// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <stdexcept>

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
void Brainfreeze::write(Interpreter::byte_t d)
{
    std::cout << static_cast<char>(d);
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::byte_t Brainfreeze::read()
{
    Interpreter::byte_t d;

    if (std::cin >> d)
    {
        return d;
    }
    else    // if ( cin.eof() ) ... else ...
    {
        throw std::runtime_error("bad input from cin");
    }
}
