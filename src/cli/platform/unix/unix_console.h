// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "../console.h"

#include <unistd.h>
#include <termios.h>

namespace Brainfreeze::CommandLineApp
{
    /** Generic UNIX console implementation. */
    class UnixConsole : public Console
    {
    public:
        using Console::write;

        /** Default constructor. */
        UnixConsole();

        /** Destructor. */
        virtual ~UnixConsole();

        /** Write a byte to the console. */
        virtual void write(char d, OutputStreamName stream) override;

        /** Write a string to standard output. */
        virtual void write(std::string_view message, OutputStreamName stream) override;

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

        /** Set if console should handle raw unbuffered console input. */
        virtual void setInputBuffering(bool isEnabled) override;

        /** Get if console input buffering is handled. */
        virtual bool isInputBufferingEnabled() const noexcept override;

        /** Set if characters are echoed when typed. */
        virtual void setInputEchoing(bool isEnabled) override;

        /** Get if characters are echoed when typed. */
        virtual bool isInputEchoingEnabled() const noexcept override;

    private:
        /** Print a terminal control code to any valid unredirected output stream. */
        void printControlCode(const char* controlCode);

        /** Raises an underlying system error and displays it to the user. */
        void raiseError(int error, const char* action, const char* filename, int lineNumber);

    private:
        termios oldTerminalParams_;
        bool didChangeTerminalParams = false;
        bool isInputRedirected_ = false;
        bool isOutputRedirected_ = false;
        bool isErrorRedirected_ = false;
        bool isTextFormattingEnabled_ = true;
        bool isInputBuffered_ = false;
        bool isInputEchoed_ = false;
    };
}
