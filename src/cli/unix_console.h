// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "console.h"

#include <unistd.h>
#include <termios.h>

namespace Brainfreeze::CommandLineApp
{
    /** Generic UNIX console implementation. */
    class UnixConsole : public Console
    {
    public:
        UnixConsole();
        virtual ~UnixConsole();

        /** Write a byte to the console. */
        virtual void write(char d) override;

        /** Write a string to standard output. */
        virtual void write(std::string_view message) override;

        /** Write a string to standard output and move to the next line. */
        virtual void writeLine(std::string_view message) override;

        /** Write a string to standard error. */
        virtual void writeError(std::string_view message) override;

        /** write a string to standard error and move to the next line. */
        virtual void writeErrorLine(std::string_view message) override;

        /** Read a byte from the console. */
        virtual char read() override;

        /** Check if input is redirected from the console. */
        virtual bool isInputRedirected() const override;

        /** Check if output is redirected from the console. */
        virtual bool isOutputRedirected() const override;

        /** Check if error is redirected from the console. */
        virtual bool isErrorRedirected() const override;

        /** Set the text color for future text printed to the console. */
        virtual void setTextColor(AnsiColor foreground, AnsiColor background) override;

        /** Set the foreground color for future text printed to the console. */
        virtual void setTextForegroundColor(AnsiColor color) override;

        /** Set the background color for future text printed to the console. */
        virtual void setTextBackgroundColor(AnsiColor color) override;

        /** Set or remove text formatting flag for future text printed to the console. */
        virtual void setTextFormat(AnsiFormatOption option, bool shouldEnable = true) override;

        /** Remove all text formatting flags for future text printed to the console. */
        virtual void resetTextFormatting() override;

        /** Reset text color to colors at console initialization. */
        virtual void resetTextColors() override;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextForegroundColor() override;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextBackgroundColor() override;

        /** Set window title. */
        virtual void setTitle(std::string_view title) override;

    private:
        termios oldTerminalParams_;
        bool isInputRedirected_ = false;
        bool isOutputRedirected_ = false;
        bool isErrorRedirected_ = false;
    };
}
