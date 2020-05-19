// Copyright 2009-2020, Scott MacDonald.
#define DEBUG_TRACE_LOGGING

#include "unix_console.h"
#include "../exceptions.h"

#include <array>
#include <cassert>

#include <stdio.h>
#include <errno.h>

using namespace Brainfreeze;
using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
namespace
{
    /** Details for each color code supported by ANSI terminal standard. */
    struct ansi_color_info_t
    {
        ansi_color_info_t(
                const char* name,
                const char* foreground,
                const char* background)
            : name(name),
              foreground(foreground),
              background(background)
        {
        }

        const char* name;
        const char* foreground;
        const char* background;
    };

    /** Details for each format mode supported by ANSI terminal standard. */
    struct ansi_format_info_t
    {
        ansi_format_info_t(
                const char* name,
                const char* enable,
                const char* disable)
            : name(name),
              enable(enable),
              disable(disable)
        {
        }

        const char* name;
        const char* enable;
        const char* disable;
    };

    /** Table of colors mapped to their ansi terminal escape codes. */
    std::array<ansi_color_info_t, AnsiColorCount> GAnsiColorTable
    {
        ansi_color_info_t { "Black",        "\033[30m", "\033[40m" },
        ansi_color_info_t { "DarkRed",      "\033[31m", "\033[41m" },
        ansi_color_info_t { "DarkGreen",    "\033[32m", "\033[42m" },
        ansi_color_info_t { "DarkYellow",   "\033[33m", "\033[43m" },
        ansi_color_info_t { "DarkBlue",     "\033[34m", "\033[44m" },
        ansi_color_info_t { "DarkMagenta",  "\033[35m", "\033[45m" },
        ansi_color_info_t { "DarkCyan",     "\033[36m", "\033[46m" },
        ansi_color_info_t { "BrightGray",   "\033[37m", "\033[47m" },
        ansi_color_info_t { "DarkGray",     "\033[90m", "\033[100m" },
        ansi_color_info_t { "LightRed",     "\033[91m", "\033[101m" },
        ansi_color_info_t { "LightGreen",   "\033[92m", "\033[102m" },
        ansi_color_info_t { "LightYellow",  "\033[93m", "\033[103m" },
        ansi_color_info_t { "LightBfalseue","\033[94m", "\033[104m" },
        ansi_color_info_t { "LightMagenta", "\033[95m", "\033[105m" },
        ansi_color_info_t { "LightCyan",    "\033[96m", "\033[106m" },
        ansi_color_info_t { "White",        "\033[97m", "\033[107m" },
    };

    /** Table of formating options mapped to their ansi terminal escape codes. */
    std::array<ansi_format_info_t, AnsiFormatOptionCount> GAnsiFormatOptionTable
    {
        ansi_format_info_t { "Blink",     "\033[5m", "\033[25m" },
        ansi_format_info_t { "Bold",      "\033[1m", "\033[22m" },
        ansi_format_info_t { "Crossout",  "\033[9m", "\033[29m" },
        ansi_format_info_t { "Invert",    "\033[7m", "\033[27m" },
        ansi_format_info_t { "Underline", "\033[4m", "\033[24m" }
    };
}

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::UnixConsole()
    : Console()
{
    // Check if standard input/output streams have been redirected.
    isInputRedirected_ = !isatty(STDIN_FILENO);
    isOutputRedirected_ = !isatty(STDOUT_FILENO);
    isErrorRedirected_ = !isatty(STDERR_FILENO);

    if (!isInputRedirected())
    {
        // Save old terminal parameters so they can be restored when this console instance is destroyed.
        if (tcgetattr(0, &oldTerminalParams_) != 0)
        {
            // Failed to save old terminal parameters.
            raiseError(errno, "Failed to save terminal parameters", __FILE__, __LINE__);
        }
    }
    
    // Disable output buffering.
    if (!isOutputRedirected())
    {
        setbuf(stdout, NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::~UnixConsole()
{
    // Restore old terminal parameters now that the console is being destroyed.
    if (didChangeTerminalParams)
    {
        tcsetattr(0, TCSADRAIN, &oldTerminalParams_);
    }

    // Restore terminal output color and formatting parameters.
    if (isTextFormattingEnabled_)
    {
        resetTextColors();
        resetTextFormatting();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(char d, OutputStreamName stream)
{
    auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);
    int status = 0;

    if (d == '\n' && shouldConvertOutputLFtoCRLF())
    {
        status = fprintf(handle, "\r\n");
    }
    else
    {
        status = fprintf(handle, "%c", d);
    }

    if (status < 0)
    {
        raiseError(errno, "Writing a character", __FILE__, __LINE__);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(std::string_view message, OutputStreamName stream)
{
    // TODO: support LF -> CRLF option for newlines found in message.
    auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);

    if (fprintf(handle, "%.*s", (int)message.size(), message.data()) < 0)
    {
        raiseError(errno, "Writing a string", __FILE__, __LINE__);
    }
}

//---------------------------------------------------------------------------------------------------------------------
char UnixConsole::read()
{
    // TODO: support CRLF -> LF option.
    // Read a single (unbuffered) character from standard in.
    char c = EOF;

    if (::read(0, &c, 1) < 0)
    {
        raiseError(errno, "Reading a character", __FILE__, __LINE__);
    }

    // Echo the character if requested.
    if (shouldEchoCharForInput())
    {
        write(c);
    }

    // Return character.
    return c;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isInputRedirected() const
{
    return isInputRedirected_;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isOutputRedirected() const
{
    return isOutputRedirected_;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isErrorRedirected() const
{
    return isErrorRedirected_;
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextColor(AnsiColor foreground, AnsiColor background)
{
    setTextForegroundColor(foreground);
    setTextBackgroundColor(background);
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextForegroundColor(AnsiColor color)
{
    if (isTextFormattingEnabled_)
    {
        printControlCode(GAnsiColorTable[(size_t)color].foreground);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextBackgroundColor(AnsiColor color)
{
    if (isTextFormattingEnabled_)
    {
        printControlCode(GAnsiColorTable[(size_t)color].background);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextFormat(AnsiFormatOption option, bool shouldEnable)
{
    if (isTextFormattingEnabled_)
    {
        // Get the correct control code to either enable or disable the option.
        const auto& opt = GAnsiFormatOptionTable[(size_t)option];
        auto code = (shouldEnable ? opt.enable : opt.disable);

        // Now print the format control code to the terminal.
        printControlCode(code);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextFormatting()
{
    for (size_t i = 0; i < AnsiFormatOptionCount; ++i)
    {
        setTextFormat(static_cast<AnsiFormatOption>(i), false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextColors()
{
    resetTextForegroundColor();
    resetTextBackgroundColor();
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextForegroundColor()
{
    if (isTextFormattingEnabled_)
    {
        printControlCode("\033[39m");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextBackgroundColor()
{
    if (isTextFormattingEnabled_)
    {
        printControlCode("\033[49m");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTitle(std::string_view /*title*/)
{
    // TODO: Implement stub.
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::printControlCode(const char* controlCode)
{
    assert(controlCode != nullptr);

    // Only print the control code if there is a valid raw output or error stream.
    if (isRawOutput() || isRawError())
    {
        // Prefer printing to standard out unless it was disabled in which case print to stderr.
        auto handle = (isRawOutput() ? stdout : stderr);
        fprintf(handle, "%s", controlCode);
    }   
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::raiseError(int error, const char* action, const char* filename, int lineNumber)
{
    auto message = POSIXException::getMessageForError(error);

    if (isTextFormattingEnabled_ && isRawError())
    {
        fprintf(stderr, "\033[31m");
    }

    fprintf(stderr, "[CONSOLE] %s: %s in %s:%d", action, message.c_str(), filename, lineNumber);

    if (isTextFormattingEnabled_ && isRawError())
    {
        fprintf(stderr, "\033[39m");
    }

    fprintf(stderr, "\n");
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setInputBuffering(bool isEnabled)
{
    // Do not apply any terminal changes if input was redirected.
    if (isInputRedirected())
    {
        return;
    }

    // Ref: https://stackoverflow.com/a/20482594
    // Get current terminal parameters.
    termios terminalParams;

    if (tcgetattr(0, &terminalParams) != 0)
    {
        raiseError(errno, "Failed to get terminal parameters", __FILE__, __LINE__);
        return;
    }

    // Set/unset cannonical mode to set/unset input buffering.
    // TODO: Isn't this backwards?
    if (isEnabled)
    {
        terminalParams.c_lflag &= ~ICANON;
        terminalParams.c_cc[VMIN] = 1;  // Read that blocks until at least one char is available.
        terminalParams.c_cc[VTIME] = 0;
    }
    else
    {
        terminalParams.c_lflag |= ICANON;
    }
    
    // Apply changed terminal paramaters.
    if (tcsetattr(0, TCSANOW, &terminalParams) != 0)
    {
        raiseError(errno, "Failed to set console input buffering (cannonical mode)", __FILE__, __LINE__);
        return;
    }

    isInputBuffered_ = isEnabled;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isInputBufferingEnabled() const noexcept
{
    return isInputBuffered_;
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setInputEchoing(bool isEnabled)
{
    IConsole::setShouldEchoCharForInput(isEnabled);
    
    // Do not apply any terminal changes if input was redirected.
    if (isInputRedirected())
    {
        return;
    }

    // Ref: https://stackoverflow.com/a/20482594
    // Get current terminal parameters.
    termios terminalParams;

    if (tcgetattr(0, &terminalParams) != 0)
    {
        raiseError(errno, "Failed to get terminal parameters", __FILE__, __LINE__);
        return;
    }

    // Set/unset cannonical mode to set/unset input buffering.
    // TODO: Isn't this backwards?
    if (isEnabled)
    {
        terminalParams.c_lflag &= ~ECHO;
    }
    else
    {
        terminalParams.c_lflag |= ECHO;
    }
    
    // Apply changed terminal paramaters.
    if (tcsetattr(0, TCSANOW, &terminalParams) != 0)
    {
        raiseError(errno, "Failed to set console input echoing", __FILE__, __LINE__);
        return;
    }

    isInputEchoed_ = isEnabled;
}

//---------------------------------------------------------------------------------------------------------------------
bool UnixConsole::isInputEchoingEnabled() const noexcept
{
    return isInputEchoed_;
}
