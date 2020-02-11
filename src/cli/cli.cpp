// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"
#include "argparser.h"

#include <iostream>
#include <fstream>
#include <cassert>

using namespace Brainfreeze::ArgParsing;

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

int unguardedMain(int argc, const char** argv)
{
    // Configure command line arguments.
    // TODO: Support specifying input file w/out the -f flag.
    // TODO: Support passing BF code inline
    // TODO: Support reading from standard in / out.
    // TODO: Support setting input / output as files.
    // TODO: Take unbound input as file input.
    ArgParser argparser;

    //argparser.setDescription("Runs Brainfreeze programs");

    std::string inputFilePath;
    size_t cellCount = 30000;           // TODO: Use to configure interpreter.
    size_t blockSize = 1;               // TODO: Use to configure interpreter.

    try
    {
        // Parse command line options.
        argparser.addOption("help").shortName('h')
            .description("Show this help message and exit");
        argparser.addOption("version").shortName('v')
            .description("Show version information and exit");
        argparser.addOption("file").shortName('f')
            .expectsString(&inputFilePath)
            .positional()
            .description("Path to Brainfreeze program");
        argparser.addOption("cells")
            .expectsSize(&cellCount)
            .description("Number of memory cells to allocate");
        argparser.addOption("blocksize")
            .expectsSize(&blockSize)
            .description("Size of each memory cell in bytes (1, 2 or 4)");

        auto args = argparser.parse(argc, argv);

        // Execute depending on command line options.
        if (args->option("help").wasSet())
        {
            printHelp();
        }
        else if (args->option("version").wasSet())
        {
            printVersionInfo();
        }
        else if (args->option("file").wasSet())
        {
            auto scriptFilePath = args->option("file").argumentValue();

            // Load code from disk.
            // TODO: Manually load the code, and then configure the compiler instead of this oneshot function.
            // TODO: Configure interpeter with command line options.
            // TODO: Print an error if code failed to load.
            // TODO: Print errors from code along with line/column and highlighting.
            auto interpreter = Brainfreeze::Helpers::LoadFromDisk(scriptFilePath);
            interpreter->run();
        }
        else
        {
            std::cerr << "No action or program given" << std::endl;
            printHelp();
        }
    }
    catch (const ArgParserException & e)
    {
        std::cerr << e.message() << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, const char* argv[])
{
    try
    {
        return unguardedMain(argc, argv);
    }
    catch (const std::exception & e)
    {
        std::cerr << "UNHANDLED EXCEPTION: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
