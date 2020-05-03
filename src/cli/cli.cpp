// Copyright 2009-2020, Scott MacDonald.
#include "bf/bf.h"
#include "bf/exceptions.h"
#include "bf/helpers.h"

#include "platform/console.h"
#include "platform/exception.h"

#include <CLI11/CLI11.hpp>
#include <loguru/loguru.hpp>

#include <sstream>
#include <fstream>
#include <system_error>

using namespace Brainfreeze;
using namespace Brainfreeze::CommandLineApp;

std::unique_ptr<Console> GConsole = nullptr;

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
    auto console = CreateConsole();

    console->setShouldConvertInputCRtoLF(convertInputCRLF);
    console->setShouldConvertOutputLFtoCRLF(convertOutputLF);
    console->setTitle(std::string("Brainfreeze: " + inputFilePath));

    try
    {
        // Load code from disk.
        // TODO: Print errors from code along with line/column and highlighting.
        // TODO: Make the compiler configurable (like optimizations).
        auto interpreter = Brainfreeze::Helpers::LoadFromDisk(inputFilePath);

        interpreter->setCellCount(cellCount);
        interpreter->setCellSize(blockSize);
        interpreter->setEndOfStreamBehavior(endOfStreamBehavior);
        interpreter->setConsole(std::move(console));
        
        // Now execute the program
        interpreter->run();
    }
    catch (const CompileException& e)
    {
        // TODO: Print out the line and highlight the character causing the problem.
        std::stringstream ss;

        ss << inputFilePath << "(" << e.lineNumber() << "): " << e.what() << std::endl;
        ss << "Execution terminated early because of compile errors" << std::endl;

        GConsole->setTextForegroundColor(AnsiColor::DarkRed, OutputStreamName::Stderr);
        GConsole->write(ss.str());

        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Initialize logger.
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;     // Disable writing to stderr. TODO: It still writes.
    loguru::init(argc, argv);

    loguru::add_file("output.log", loguru::Truncate, loguru::Verbosity_2);    // TODO: Make this configurable.

    // Enter program main.
    try
    {
        // Initialize console singleton.
        GConsole = CreateConsole();

        return unguardedMain(argc, argv);
    }
    catch (const PlatformException& e)
    {
        std::cerr << "*** UNHANDLED EXCEPTION ***" << std::endl;
        std::cerr << e.originalMessage() << std::endl;
        
        if (e.hasSourceFileName())
        {
            std:: cerr << e.sourceFileName() << ":" << e.sourceLineNumber() << std::endl;
        }

        // TODO: Rwork this so it sets color and stuff even if GConsole is not initialized.
        //GConsole->setTextForegroundColor(AnsiColor::DarkRed, OutputStreamName::Stderr);
        //GConsole->write(ss.str());
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "*** UNHANDLED EXCEPTION ***" << std::endl;
        std::cerr << e.what() << std::endl;

        // TODO: Rwork this so it sets color and stuff even if GConsole is not initialized.
        //GConsole->setTextForegroundColor(AnsiColor::DarkRed, OutputStreamName::Stderr);
        //GConsole->write(ss.str());
        return EXIT_FAILURE;
    }
}
