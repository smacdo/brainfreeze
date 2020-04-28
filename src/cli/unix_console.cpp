// Copyright 2009-2020, Scott MacDonald.
#include "unix_console.h"
#include "bf/bf.h"

#include <stdio.h>

using namespace Brainfreeze;
using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::UnixConsole()
{
    // TODO: Test if standard in, out has been redirected.

    // Save old terminal parameters so they can be restored when this console instance is destroyed.
    // TODO: Don't save and modify terminal if standard in was redirected.
    if (tcgetattr(0, &oldTerminalParams_))
    {
        // TODO: Capture unix error codes and maybe print real error messages.
        throw std::runtime_error("TODO get error and report that tcgetattr broken");
    }

    // Disable echo and cannonical mode to get unbuffered character input support.
    auto terminalParams = oldTerminalParams_;

    terminalParams.c_lflag &= ~ICANON;
    terminalParams.c_lflag &= ~ECHO;
    terminalParams.c_cc[VMIN] = 1;
    terminalParams.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &terminalParams) != 0)
    {
        // TODO: Capture unix error codes and maybe print real error messages.
        throw std::runtime_error("something went wrong with tcsetattr");
    }

    // Disable output buffer.
    // TODO: Only do this if standard out was not redirected.
    setbuf(stdout, NULL);
}

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::~UnixConsole()
{
    // Restore old terminal parameters now that the console is being destroyed.
    // TODO: Only do this if standard in was not redirected.
    tcsetattr(0, TCSADRAIN, &oldTerminalParams_);
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(char d)
{
    // TODO: Finish stub implementation.
    printf("%c", d);
}

//---------------------------------------------------------------------------------------------------------------------
char UnixConsole::read()
{
    // TODO: Handle input redirection. Maybe buffering?
    char c = EOF;

    if (::read(0, &c, 1) == -1)
    {
        // TODO: Capture unix error codes and maybe print real error messages.
        throw std::runtime_error("something went wrong with read");
    }

    // TODO: Only echo if requested.
    write(c);

    return c;
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
