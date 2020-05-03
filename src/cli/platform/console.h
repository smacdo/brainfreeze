// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "bf/iconsole.h"

#include <string_view>
#include <memory>

namespace Brainfreeze::CommandLineApp
{
    /** List of colors supported by ANSI terminal standard. */
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

    constexpr const size_t AnsiColorCount = static_cast<size_t>(AnsiColor::White) + 1;

    /** List of text formatting options supported by ANSI terminal standard. */
    enum class AnsiFormatOption
    {
        Blink = 0,
        Bold = 1,
        Crossout = 2,
        Invert = 3,
        Underline = 4
    };

    constexpr const size_t AnsiFormatOptionCount = static_cast<size_t>(AnsiFormatOption::Underline) + 1;

    /** Output stream name. */
    enum class OutputStreamName
    {
        Stdout,
        Stderr
    };

    /** Cross platform console interface. */
    class Console : public IConsole
    {
    public:
        /** Write a string to standard output. */
        virtual void write(
            std::string_view message,
            OutputStreamName stream = OutputStreamName::Stdout) = 0;

        /** Write a string to standard output and move to the next line. */
        virtual void writeLine(
            std::string_view message,
            OutputStreamName stream = OutputStreamName::Stdout) = 0;

        /** Check if input is redirected from the console. */
        virtual bool isInputRedirected() const = 0;

        /** Check if output is redirected from the console. */
        virtual bool isOutputRedirected() const = 0;

        /** Check if error is redirected from the console. */
        virtual bool isErrorRedirected() const = 0;

        /** Set the text color for future text printed to the console. */
        virtual void setTextColor(
            AnsiColor foreground,
            AnsiColor background,
            OutputStreamName stream = OutputStreamName::Stdout) = 0;

        /** Set the foreground color for future text printed to the console. */
        virtual void setTextForegroundColor(
            AnsiColor color,
            OutputStreamName stream = OutputStreamName::Stdout) = 0;

        /** Set the background color for future text printed to the console. */
        virtual void setTextBackgroundColor(
            AnsiColor color,
            OutputStreamName stream = OutputStreamName::Stdout) = 0;

        /** Set or remove text formatting flag for future text printed to the console. */
        virtual void setTextFormat(AnsiFormatOption option, bool shouldEnable = true) = 0;

        /** Remove all text formatting flags for future text printed to the console. */
        virtual void resetTextFormatting() = 0;

        /** Reset text color to colors at console initialization. */
        virtual void resetTextColors() = 0;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextForegroundColor() = 0;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextBackgroundColor() = 0;

        /** Set window title. */
        virtual void setTitle(std::string_view title) = 0;
    };

    /** Create a new instance of the appropriate platform specific console. */
    std::unique_ptr<Console> CreateConsole();
}