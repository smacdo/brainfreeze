// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<Interpreter> Brainfreeze::Helpers::LoadFromDisk(const std::string& filename)
{
    // Open the text file and read it into a file line by line.
    // NOTE: If performance ever becomes an issue (doubtful with Brainfreeze programs being so small) this function can
    //       be optimized to get the file size, allocate a buffer upfront and then read everything into it.
    std::ifstream infile;
    std::stringstream sstream;
    std::string line;

    infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    infile.open(filename.c_str());

    while (std::getline(infile, line, '\n'))
    {
        sstream << line;
    }

    std::string code = sstream.str();

    // Compile the code into an instruction stream, and then return an interpreter with the instruction stream loaded.
    Compiler compiler;
    return std::make_unique<Interpreter>(compiler.compile(code));
}
