// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "../console.h"
#include <Windows.h>
#include <queue>
#include <optional>

namespace Brainfreeze::CommandLineApp
{
    /** Windows specific console implementation. */
    class WindowsConsole : public Console
    {
    public:
        WindowsConsole();
        virtual ~WindowsConsole();

        /** Write a byte to the Windows console. */
        virtual void write(char d, OutputStreamName stream) override;

        /** Write a string to standard output. */
        virtual void write(std::string_view message, OutputStreamName streamName) override;

        /** Write a string to standard output and move to the next line. */
        virtual void writeLine(std::string_view message, OutputStreamName streamName) override;

        /** Read a byte from the Windows console. */
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
        void readBuffered(bool waitForCharacter = false);
        void clearInputBuffer();
        void raiseError(DWORD error, const char* action, const char* filename, int lineNumber);

        struct stream_data_t;

        void initStream(stream_data_t* streamData, HANDLE standardHandle, bool isOutputHandle);
        void restoreStreamPrevState(const stream_data_t& stream);

    private:
        static const size_t DefaultBufferSize = 256;

        /// Internal buffer of characters read from the input device.
        std::queue<char> bufferedChars_;

        /// Get if input is line buffered (true) or unbuffered (false).
        bool bIsInputBuffered_ = true;

        /// Get if input is echoed (true) or not echoed (false).
        bool bIsInputEchoed_ = true;

    private:
        struct stream_data_t
        {
            bool isConsole = false;
            bool isOutput = false;
            HANDLE handle = INVALID_HANDLE_VALUE;
            DWORD prevMode = 0;
            WORD outputCharAttributes = 0;  // TODO: prevXX
        };

        stream_data_t inputStream_;
        stream_data_t outputStream_;
        stream_data_t errorStream_;

        AnsiColor currentTextForegroundColor_ = AnsiColor::Black;
        AnsiColor currentTextBackgroundColor_ = AnsiColor::Black;
        std::string windowTitle_;
    };
}
