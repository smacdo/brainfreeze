// Copyright 2009-2020, Scott MacDonald.
#include "unix_console.h"
#include "bf/bf.h"

#include <loguru/loguru.hpp>
#include <array>
#include <stdexcept>
#include <cassert>

using namespace Brainfreeze;
using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::UnixConsole() = default;

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::~UnixConsole() = default;

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(char d)
{
    // TODO: Finish stub implementation.
    printf("%c", d);
}

//---------------------------------------------------------------------------------------------------------------------
char UnixConsole::read()
{
    // TODO: Implement stub.
    return EOF;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isInputRedirected() const
{
    // TODO: Implement stub.
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isOutputRedirected() const
{
    // TODO: Implement stub.
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextColor(AnsiColor /*foreground*/, AnsiColor /*background*/)
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextForegroundColor(AnsiColor /*color*/)
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextBackgroundColor(AnsiColor /*color*/)
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextColors()
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextForegroundColor()
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextBackgroundColor()
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTitle(std::string_view /*title*/)
{
    // TODO: Implement stub.
}
