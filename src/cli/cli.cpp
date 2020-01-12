// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <iostream>
#include <fstream>

using namespace Brainfreeze;

/**
 * Summons forth helpful version information along with licensing and
 * copyright information.
 */
void printVersionInfo()
{
    std::cout
        << "Brainfreeze Interpreter " << Brainfreeze::Version << std::endl
        << "Copyright (C) 2009-2020 Scott MacDonald." << std::endl
        << "License GPLv2: GNU GPL version 2 "
        << "<http://www.gnu.org/licenses/gpl.html>" << std::endl
        << "This is free software: you are free to change it and redistribute "
        << "it." << std::endl
        << "There is NO WARRANTY, to the extent permitted by law." << std::endl
        << std::endl;
}

/**
 * Prints a big chunk of descriptive text detailing the options available
 * to the user when running the brainfreeze command line interpreter. Usually
 * called when they screw up their input :)
 *
 * \param descr The program options that were configured for the command line
 */
void printHelp()
{
    std::cout
        << "Usage: brainfreeze [options] [scriptfile]" << std::endl
        << "" << std::endl << std::endl
        << "Report bugs to: bugs@whitespaceconsideredharmful.com" << std::endl
        << "Website: http://whitespaceconsideredharmful.com/bf"
        << std::endl << std::endl;
}

int main( int, char*[] )
{
    //
    // Create our list of allowable arguments
    //
    //unsigned int memoryBlockCount, memoryBlockSize, eolValue;
   
    // Command line only options
/*    po::options_description generic("Generic Options");
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
        printHelp( generic );
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

    if ( false == BF::loadFromDisk( scriptfile, contents ) )
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
    */
    // It worked! woohoo
    return 0;
}
