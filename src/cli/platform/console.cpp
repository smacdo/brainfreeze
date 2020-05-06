// Copyright 2009-2020, Scott MacDonald.
#include "console.h"
#if _WIN32
#   include "windows/windows_console.h"
#else
#   include "unix/unix_console.h"
#endif

using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
Console::~Console() = default;

//---------------------------------------------------------------------------------------------------------------------
void Console::write(char c)
{
    write(c, OutputStreamName::Stdout);
}

//---------------------------------------------------------------------------------------------------------------------
void Console::write(std::string_view message)
{
    write(message, OutputStreamName::Stdout);
}

//---------------------------------------------------------------------------------------------------------------------
void Console::writeLine(std::string_view message)
{
    writeLine(message, OutputStreamName::Stdout);
}

//---------------------------------------------------------------------------------------------------------------------
void Console::writeLine(std::string_view message, OutputStreamName stream)
{
    write(message, stream);
    write('\n', stream);
}

//---------------------------------------------------------------------------------------------------------------------
bool Console::isRawInput() const
{
    return !isInputRedirected();
}

//---------------------------------------------------------------------------------------------------------------------
bool Console::isRawOutput() const
{
    return !isOutputRedirected();
}

//---------------------------------------------------------------------------------------------------------------------
bool Console::isRawError() const
{
    return !isErrorRedirected();
}

//---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<Console> Brainfreeze::CommandLineApp::CreateConsole()
{
#if _WIN32
    return std::make_unique<WindowsConsole>();
#else
    return std::make_unique<UnixConsole>();
#endif
}
