// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"
#include "windows_console.h"        // TODO: Make one for each platform

#include <CLI11/CLI11.hpp>
#include <loguru/loguru.hpp>

#include <iostream>
#include <fstream>
#include <cassert>

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

int unguardedMain(int argc, char** argv)
{
    // Configure command line arguments.
    // TODO: Support specifying input file w/out the -f flag.
    // TODO: Support passing BF code inline
    // TODO: Support reading from standard in / out.
    // TODO: Support setting input / output as files.
    // TODO: Take unbound input as file input.
    CLI::App app;

    app.description("Executes brainfreeze programs (a nicer brainf*ck) from the command line");

    //argparser.setAppCopyright("Brainfreeze (c) 2020 Scott MacDonald");


    std::string inputFilePath;
    size_t cellCount = 30000;           // TODO: Use to configure interpreter.
    size_t blockSize = 1;               // TODO: Use to configure interpreter.

    // Parse command line options.
    app.add_option("-f,--file,file", inputFilePath, "Path to Brainfreeze program")->required();
    app.add_option("--cells", cellCount, "Number of memory cells to allocate");     // TODO: Validate.
    app.add_option("--blockSize", blockSize, "Size of each memory cell in bytes (1, 2, or 4)"); // TODO: Validate.

    // TODO: Add ability to set null console or read from file or something.

    CLI11_PARSE(app, argc, argv);

    // Load code from disk.
    // TODO: Manually load the code, and then configure the compiler instead of this oneshot function.
    // TODO: Configure interpeter with command line options.
    // TODO: Print an error if code failed to load.
    // TODO: Print errors from code along with line/column and highlighting.
    auto interpreter = Brainfreeze::Helpers::LoadFromDisk(inputFilePath);

    // TODO: Set platform specific console.
    interpreter->setConsole(std::make_unique<WindowsConsole>());

    interpreter->run();
    
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;     // Disable writing to stderr.
    loguru::init(argc, argv);
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;     // Disable writing to stderr.
    loguru::add_file("output.log", loguru::Truncate, loguru::Verbosity_MAX);    // TODO: Make this configurable.

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
