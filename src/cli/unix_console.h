// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "console.h"

namespace Brainfreeze::CommandLineApp
{
    /** Generic UNIX console implementation. */
    class UnixConsole : public Console
    {
    public:
        UnixConsole();
        virtual ~UnixConsole();

        /** Write a byte to the Windows console. */
        virtual void write(char d) override;

        /** Read a byte from the Windows console. */
        virtual char read() override;

        /** Check if input is redirected from the console. */
        virtual bool isInputRedirected() const override;

        /** Check if output is redirected from the console. */
        virtual bool isOutputRedirected() const override;

        /** Set the text color for future text printed to the console. */
        virtual void setTextColor(AnsiColor foreground, AnsiColor background) override;

        /** Set the foreground color for future text printed to the console. */
        virtual void setTextForegroundColor(AnsiColor color) override;

        /** Set the background color for future text printed to the console. */
        virtual void setTextBackgroundColor(AnsiColor color) override;

        /** Reset text color to colors at console initialization. */
        virtual void resetTextColors() override;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextForegroundColor() override;

        /** Reset text foreground color to foreground color at console initialization. */
        virtual void resetTextBackgroundColor() override;

        /** Set window title. */
        virtual void setTitle(std::string_view title) override;
    };
}
