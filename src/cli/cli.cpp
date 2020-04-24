// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"
#include "windows_console.h"        // TODO: Make one for each platform

#include <CLI11/CLI11.hpp>
#include <loguru/loguru.hpp>

#include <iostream>
#include <fstream>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
int unguardedMain(int argc, char** argv)
{
    CLI::App app;

    app.description("Run Brainfuck programs from the command line");
    app.footer(
        "Copyright (c) Scott MacDonald.\n"
        "For support please see: https://github.com/smacdo/brainfreeze \n");
   
    // Command line options.
    const std::map<std::string, Interpreter::EndOfStreamBehavior> EOSLookupTable({
        {"zero", Interpreter::EndOfStreamBehavior::Zero},
        {"negativeOne", Interpreter::EndOfStreamBehavior::NegativeOne},
        {"nochange", Interpreter::EndOfStreamBehavior::NoChange}
    });

    std::string inputFilePath;
    auto endOfStreamBehavior = Interpreter::EndOfStreamBehavior::NegativeOne;

    size_t cellCount = 30000;           // TODO: Use to configure interpreter.
    size_t blockSize = 1;               // TODO: Use to configure interpreter.

    bool convertInputCRLF = false;      // TODO: Set default per platform.
    bool convertOutputLF = false;

    app.add_option("-f,--file,file", inputFilePath)
        ->description("Path to Brainfreeze program")
#if _WIN32
        ->type_name("<path\\to\\file.bf>")
#else
        ->type_name("<path/to/file.bf>")
#endif
        ->required();

    app.add_option("--cells", cellCount)
        ->description("Number of memory cells")
        ->type_name("<number>");

    app.add_set("--blockSize", blockSize, { 1, 2, 4, 8 })
        ->description("Size of each memory cell in bytes")
        ->type_name("<number>");

    app.add_option("--eof", endOfStreamBehavior)
        ->description("End of stream behavior")
        ->type_name("<behavior>")
        ->ignore_case()
        ->ignore_underscore()
        ->transform(CLI::CheckedTransformer(EOSLookupTable, CLI::ignore_case));

    app.add_flag("--convertInputCRLF", convertInputCRLF)
        ->description("Convert Windows style newlines (\\r\\n) to *nix (\\n) when reading input.")
#if _WIN32
        ->default_val(true)
#else
        ->default_val(false)
#endif
        ->ignore_case();

    app.add_flag("--convertOutputLF", convertOutputLF)
        ->description("Convert *nix newlines (\\n) to Windows (\\r\\n) when writing output.")
#if _WIN32
        ->default_val(true)
#else
        ->default_val(false)
#endif
        ->ignore_case();

    // Parse command line options.
    CLI11_PARSE(app, argc, argv);

    // Instantiate a platform specific console handler for reading standard in and writing standard out.
#if _WIN32
    auto console = std::make_unique<WindowsConsole>();
#else
#error  "TODO: Implement platform specific console"
#endif

    console->setShouldConvertInputCRtoLF(convertInputCRLF);
    console->setShouldConvertOutputLFtoCRLF(convertOutputLF);

    // Load code from disk.
    // TODO: Print an error if code failed to load.
    // TODO: Print errors from code along with line/column and highlighting.
    auto interpreter = Brainfreeze::Helpers::LoadFromDisk(inputFilePath);

    interpreter->setCellCount(cellCount);
    interpreter->setCellSize(blockSize);
    interpreter->setEndOfStreamBehavior(endOfStreamBehavior);
    interpreter->setConsole(std::move(console));

    interpreter->run();
    
    return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;     // Disable writing to stderr. TODO: It still writes.
    loguru::init(argc, argv);

    loguru::add_file("output.log", loguru::Truncate, loguru::Verbosity_0);    // TODO: Make this configurable.

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
