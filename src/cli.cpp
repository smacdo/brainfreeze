/**
 * Brainfreeze Language Interpreter
 * (c) 2009 Scott MacDonald. All rights reserved.
 *
 * Command line interface to the interpreter
 */
#include "bf.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sstream>

const char * BF_EXAMPLE = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

/**
 * Loads from disk
 */
bool loadFromDisk( const std::string& filename,
						 std::string& filedata )
{
	std::ifstream infile;
	std::string   line;
	std::stringstream str;
	
	// Try to open it
	infile.open(filename.c_str());
	
	// Did it open?
	if ( infile.is_open() == false || infile.bad()  )
	{
		perror("Failed to open file");
		return false;
	}
	
	// read the file in
	while( std::getline( infile, line, '\n' ) )
	{
		str << line;
	}
	
	infile.close();
	
	// Save the file data, and return
	filedata = str.str();
	return true;
}

/**
 * Prints version information to the console
 */
void printVersionInfo()
{
    std::cout << "Brainfreeze 0.1 (build 349)" << std::endl;
}

int main( int argc, char * argv[] )
{
	std::string code;

//    runTests();

    //
    // Command line parsing
    //
    FILE * input  = stdin;
    FILE * output = stdout;

    for ( int i = 1; i < argc; ++i )
    {
        char * arg = argv[i];

        // Version?
        if ( strcmp( arg, "-v" ) == 0 ||
             strcmp( arg, "--version" ) == 0 )
        {
            printVersionInfo();
            return 0;
        }
        else
        {
            // Must be a file then
            if ( false == loadFromDisk( arg, code ) )
            {
                std::cerr << "Could not open file: " << arg << std::endl;
                return 0;
            }
        }
    }

    BFProgram app(code);
    app.run();
}
