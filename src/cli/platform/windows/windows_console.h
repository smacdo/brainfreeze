// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "../console.h"
#include <Windows.h>
#include <queue>

namespace Brainfreeze::CommandLineApp
{
    /** Windows specific console implementation. */
    class WindowsConsole : public Console
    {
    public:
        WindowsConsole();
        virtual ~WindowsConsole();

        /** Write a byte to the Windows console. */
        virtual void write(char d) override;

        /** Write a string to standard output. */
        virtual void write(
            std::string_view message,
            OutputStreamName stream = OutputStreamName::Stdout) override;

        /** Write a string to standard output and move to the next line. */
        virtual void writeLine(
            std::string_view message,
            OutputStreamName stream = OutputStreamName::Stdout) override;

        /** Read a byte from the Windows console. */
        virtual char read() override;

        /** Check if input is redirected from the console. */
        virtual bool isInputRedirected() const override;

        /** Check if output is redirected from the console. */
        virtual bool isOutputRedirected() const override;
        
        /** Check if error is redirected from the console. */
        virtual bool isErrorRedirected() const override;

        /** Set the text color for future text printed to the console. */
        virtual void setTextColor(
            AnsiColor foreground,
            AnsiColor background,
            OutputStreamName stream = OutputStreamName::Stdout) override;

        /** Set the foreground color for future text printed to the console. */
        virtual void setTextForegroundColor(
            AnsiColor color,
            OutputStreamName stream = OutputStreamName::Stdout) override;

        /** Set the background color for future text printed to the console. */
        virtual void setTextBackgroundColor(
            AnsiColor color,
            OutputStreamName stream = OutputStreamName::Stdout) override;
            
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
        void ReadBuffered(bool waitForCharacter = false);
        void ClearInputBuffer();

    private:
        static const size_t DefaultBufferSize = 256;

        /// Internal buffer of characters read from the input device.
        std::queue<char> bufferedChars_;

        /// Standard input handle. Could be the console input buffer or a redirected input stream.
        HANDLE inputHandle_ = INVALID_HANDLE_VALUE;

        /// Standard output handle. Could be the console output buffer or a redirected output stream.
        HANDLE outputHandle_ = INVALID_HANDLE_VALUE;

        /// Get if input handle is reading from console input buffer (true) or a redirected input stream (false).
        bool bIsConsoleInput_ = true;

        /// Get if output handle is writing to console output buffer (true) or a redirected output stream (false).
        bool bIsConsoleOutput_ = true;

        /// Saved console input mode from prior to instantiating the console.
        DWORD prevConsoleInputMode_ = 0;

        /// Saved console output mode from prior to instantiating the console.
        DWORD prevConsoleOutputMode_ = 0;

        /// Saved console output character attributes from prior to instantiating the console.
        WORD prevConsoleOutputCharAttributes_ = 0;
        AnsiColor currentTextForegroundColor_ = AnsiColor::Black;
        AnsiColor currentTextBackgroundColor_ = AnsiColor::Black;
        std::string windowTitle_;
    };
}
