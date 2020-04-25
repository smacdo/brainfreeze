// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "iconsole.h"
#include <string_view>

namespace Brainfreeze::CommandLineApp
{
    /** List of ANSI escape code colors. */
    enum class AnsiColor
    {
        Black = 0,
        DarkRed = 1,
        DarkGreen = 2,
        DarkYellow = 3,
        DarkBlue = 4,
        DarkMagenta = 5,
        DarkCyan = 6,
        BrightGray = 7,
        DarkGray = 8,
        LightRed = 9,
        LightGreen = 10,
        LightYellow = 11,
        LightBlue = 12,
        LightMagenta = 13,
        LightCyan = 14,
        White = 15
    };

    constexpr size_t AnsiColorCount = static_cast<size_t>(AnsiColor::White) + 1;

    /** Cross platform console interface. */
    class Console : public IConsole
    {
    public:
        /** Check if input is redirected from the console. */
        virtual bool isInputRedirected() const = 0;

        /** Check if output is redirected from the console. */
        virtual bool isOutputRedirected() const = 0;

        /** Set the text color for future text printed to the console. */
        virtual void setTextColor(AnsiColor foreground, AnsiColor background) = 0;

        /** Set the foreground color for future text printed to the console. */
        virtual void setTextForegroundColor(AnsiColor color) = 0;

        /** Set the background color for future text printed to the console. */
        virtual void setTextBackgroundColor(AnsiColor color) = 0;

        /** Reset text color to colors at console initialization. */
        virtual void resetTextColors() = 0;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextForegroundColor() = 0;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextBackgroundColor() = 0;

        /** Set window title. */
        virtual void setTitle(std::string_view title) = 0;
    };
}