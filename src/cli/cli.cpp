// Copyright 2009-2020, Scott MacDonald.
#include "bf/bf.h"
#include "bf/exceptions.h"
#include "bf/helpers.h"

#include "platform/console.h"
#include "platform/exception.h"

#include <CLI11/CLI11.hpp>
#include <loguru/loguru.hpp>

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

    size_t cellCount = 30000;
    size_t blockSize = 1;

    bool convertInputCRLF = false;
    bool convertOutputLF = false;
    bool inputBuffering = true;
    bool shouldEchoInput = GConsole->shouldEchoCharForInput();

    app.add_option("-f,--file,file", inputFilePath)
        ->description("Path to Brainfreeze program")
#if _WIN32
        ->type_name("<path\\to\\file.bf>")
#else
        ->type_name("<path/to/file.bf>")
#endif
        ->required();

    app.add_option("-c,--cells", cellCount)
        ->description("Number of memory cells")
        ->group("Brainfuck Details")
        ->type_name("<number>");

    app.add_set("-s,--blockSize", blockSize, { 1, 2, 4, 8 })
        ->description("Size of each memory cell in bytes")
        ->group("Brainfuck Details")
        ->type_name("<number>");

    app.add_option("-e,--eof", endOfStreamBehavior)
        ->description("End of stream behavior")
        ->group("Brainfuck Details")
        ->type_name("<behavior>")
        ->ignore_case()
        ->ignore_underscore()
        ->transform(CLI::CheckedTransformer(EOSLookupTable, CLI::ignore_case));

    app.add_flag("--echoInput", shouldEchoInput)
        ->description("Write input to output for display")
        ->group("Input/Output Behavior")
        ->default_val(shouldEchoInput)
        ->ignore_case();

    app.add_flag("--inputBuffering", inputBuffering)
        ->description("Enable or disable input line buffering behavior")
        ->group("Input/Output Behavior")
        ->default_val(inputBuffering)
        ->ignore_case();

    app.add_flag("--convertInputCRLF", convertInputCRLF)
        ->description("Convert Windows style newlines (\\r\\n) to *nix (\\n) when reading input.")
        ->group("Input/Output Behavior")
#if _WIN32
        ->default_val(true)
#else
        ->default_val(false)
#endif
        ->ignore_case();

    app.add_flag("--convertOutputLF", convertOutputLF)
        ->description("Convert *nix newlines (\\n) to Windows (\\r\\n) when writing output.")
        ->group("Input/Output Behavior")
#if _WIN32
        ->default_val(true)
#else
        ->default_val(false)
#endif
        ->ignore_case();

    // Parse command line options.
    CLI11_PARSE(app, argc, argv);

    // Configure the console for Brainfreeze.
    GConsole->setShouldConvertInputCRtoLF(convertInputCRLF);
    GConsole->setShouldConvertOutputLFtoCRLF(convertOutputLF);
    GConsole->setInputBuffering(inputBuffering);
    GConsole->setInputEchoing(shouldEchoInput);
    
    GConsole->setTitle(std::string("Brainfreeze: " + inputFilePath));

    try
    {
        // Load code from disk.
        // TODO: Print errors from code along with line/column and highlighting.
        // TODO: Make the compiler configurable (like optimizations).
        auto interpreter = Brainfreeze::Helpers::LoadFromDisk(inputFilePath);

        interpreter->setCellCount(cellCount);
        interpreter->setCellSize(blockSize);
        interpreter->setEndOfStreamBehavior(endOfStreamBehavior);
        interpreter->setConsole(std::move(GConsole));   // TODO: hmmm this is a problem
        
        // Now execute the program
        interpreter->run();
    }
    catch (const CompileException& e)
    {
        GConsole->setTextForegroundColor(AnsiColor::LightRed);

        // TODO: Print out the line and highlight the character causing the problem.
        std::cerr << inputFilePath << "(" << e.lineNumber() << "): " << e.what() << std::endl;
        std::cerr << "Execution terminated early because of compile errors" << std::endl;

        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Initialize global logger.
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::init(argc, argv);

    loguru::add_file("output.log", loguru::Truncate, loguru::Verbosity_2);    // TODO: Make this configurable.

    try
    {
        // Initialize console singleton.
        GConsole = CreateConsole();
        
        // Enter program main.
        return unguardedMain(argc, argv);
    }
    catch (const PlatformException& e)
    {
        GConsole->setTextForegroundColor(AnsiColor::LightRed);
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return EXIT_FAILURE + 1;
    }
    catch (const std::exception& e)
    {
        GConsole->setTextForegroundColor(AnsiColor::LightRed);
        std::cerr << "*** UNHANDLED EXCEPTION ***" << std::endl;
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }
}
