// Copyright 2009-2020, Scott MacDonald.
#include "unix_console.h"
#include "bf/bf.h"

#include <stdio.h>
#include <errno.h>

#include <system_error>

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
        ansi_color_info_t { "LightBfalseue",    "\033[94m", "\033[104m" },
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
{
    // Check if standard input/output streams have been redirected.
    isInputRedirected_ = !isatty(STDIN_FILENO);
    isOutputRedirected_ = !isatty(STDOUT_FILENO);
    isErrorRedirected_ = !isatty(STDERR_FILENO);

    // Save input parameters and options so as to be able to restore them when this console instance is destroyed.
    if (!isInputRedirected())
    {
        // Save old terminal parameters so they can be restored when this console instance is destroyed.
        if (tcgetattr(0, &oldTerminalParams_))
        {
            throw std::system_error(errno, std::generic_category());
        }

        // Disable echo and cannonical mode to get unbuffered character input support.
        auto terminalParams = oldTerminalParams_;

        terminalParams.c_lflag &= ~ICANON;
        terminalParams.c_lflag &= ~ECHO;
        terminalParams.c_cc[VMIN] = 1;
        terminalParams.c_cc[VTIME] = 0;

        if (tcsetattr(0, TCSANOW, &terminalParams) != 0)
        {
            throw std::system_error(errno, std::generic_category());
        }
    }
    
    // Disable output buffer.
    if (!isOutputRedirected())
    {
        setbuf(stdout, NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
UnixConsole::~UnixConsole()
{
    // Restore old terminal parameters now that the console is being destroyed.
    if (!isInputRedirected())
    {
        tcsetattr(0, TCSADRAIN, &oldTerminalParams_);
    }

    // Restore terminal output color and formatting parameters.
    if (!isOutputRedirected())
    {
        resetTextColors();
        resetTextFormatting();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(char d)
{
    // TODO: support LF -> CRLF option.
    if (fprintf(stdout, "%c", d) == -1)
    {
        throw std::system_error(errno, std::generic_category());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::write(std::string_view message, OutputStreamName stream)
{
    // TODO: support LF -> CRLF option for newlines found in message.
    auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);

    if (fprintf(handle, "%s", message.data()) == -1)
    {
        throw std::system_error(errno, std::generic_category());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::writeLine(std::string_view message, OutputStreamName stream)
{
    const char * newline = shouldConvertOutputLFtoCRLF() ? "\r\n" : "\n";
    write(message);

    auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);

    if (fprintf(handle, "%s", newline) == -1)
    {
        throw std::system_error(errno, std::generic_category());
    }
}

//---------------------------------------------------------------------------------------------------------------------
char UnixConsole::read()
{
    // TODO: support CRLF -> LF option.
    // Read a single (unbuffered) character from standard in.
    char c = EOF;

    if (::read(0, &c, 1) == -1)
    {
        throw std::system_error(errno, std::generic_category());
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
void UnixConsole::setTextColor(AnsiColor foreground, AnsiColor background, OutputStreamName stream)
{
    setTextForegroundColor(foreground, stream);
    setTextBackgroundColor(background, stream);
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextForegroundColor(AnsiColor color, OutputStreamName stream)
{
    if (!isOutputRedirected())
    {
        auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);
        fprintf(handle, "%s", GAnsiColorTable[(size_t)color].foreground);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextBackgroundColor(AnsiColor color, OutputStreamName stream)
{
    if (!isOutputRedirected())
    {
        auto handle = (stream == OutputStreamName::Stdout ? stdout : stderr);
        fprintf(handle, "%s", GAnsiColorTable[(size_t)color].background);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTextFormat(AnsiFormatOption option, bool shouldEnable)
{
    if (!isOutputRedirected())
    {
        if (shouldEnable)
        {
            fprintf(stdout, "%s", GAnsiFormatOptionTable[(size_t)option].enable);
        }
        else
        {
            fprintf(stdout, "%s", GAnsiFormatOptionTable[(size_t)option].disable);
        }
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
    if (!isOutputRedirected())
    {
        fprintf(stdout, "\033[39m");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::resetTextBackgroundColor()
{
    if (!isOutputRedirected())
    {
        fprintf(stdout, "\033[49m");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void UnixConsole::setTitle(std::string_view /*title*/)
{
    // TODO: Implement stub.
}
