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

#include <boost/program_options.hpp>

namespace po = boost::program_options;

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
    //
    // Create our list of allowable arguments
    //
    unsigned int memoryBlockCount, memoryBlockSize, eolValue;
   
    // Command line only options
    po::options_description generic("Generic Options");
    generic.add_options()
        ("help",    "Show this help message and exit")
        ("script",   po::value<std::string>(),
                    "Path to brainfreeze script file")
        ("version", "Output version informaton and exit")
    ;

    po::positional_options_description pod;
    pod.add("script", -1);
 
    // Options that are allowed in both the command line and in a
    // config file
    po::options_description config("Configuration");
    config.add_options()
        ("memlength",
         po::value<unsigned int>(&memoryBlockCount)->default_value(30000),
         "Number of memory storage units for script" )
        ("memsize",
         po::value<unsigned int>(&memoryBlockSize)->default_value(8),
         "Size in bits of each memory storage unit (8,16,32)")
        ("eol",
         po::value<unsigned int>(&eolValue)->default_value(10),
         "EOL (end of line) ASCII value")
    ;

    generic.add(config);

    // Read from the command line
    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options(generic).positional(pod).run(),
               vm );
    po::notify( vm );

    // Read from an input file
    std::ifstream configfile("brainfreeze.cfg");
    po::store( po::parse_config_file( configfile, config ), vm );
    po::notify( vm );


    //
    // So what options did we get?
    //
    if ( vm.count("help") )
    {
        std::cout << "Usage: brainfreeze [options] [scriptfile]" << std::endl
                  << generic << std::endl;
        return 1;
    }
    else if ( vm.count("version") )
    {
        printVersionInfo();
        return 0;
    }
    else if ( vm.count("script") < 1 )
    {
        std::cerr << "You must specify at least on brainfreeze script to run"
                  << std::endl;
        return 0;
    }

    //
    // Load the script into memory and run that sucker
    //
    std::string contents;
    std::string scriptfile = vm["script"].as<std::string>();

    if ( false == loadFromDisk( scriptfile, contents ) )
    {
        std::cerr << "Could not open script: " << contents << std::endl;
        return 2;
    }

    BFProgram app( contents );

    if (! app.compile() )
    {
        std::cerr << "Failed to compile script: "
                  << app.errorText() << std::endl;
        return 3;
    }

    if (! app.run() )
    {
        std::cerr << "Failed to execute script: "
                  << app.errorText() << std::endl;
        return 4;
    }

    // It worked! woohoo
    return 0;
}
