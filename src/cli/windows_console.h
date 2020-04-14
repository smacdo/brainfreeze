// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "bf.h"
#include <Windows.h>
#include <queue>

namespace Brainfreeze
{
    /** Windows specific console implementation. */
    class WindowsConsole : public Console
    {
    public:
        WindowsConsole();
        virtual ~WindowsConsole();

        /** Write a byte to the Windows console. */
        virtual void Write(char d) override;

        /** Read a byte from the Windows console. */
        virtual char Read() override;

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
        bool bIsConsoleInput = true;

        /// Get if output handle is writing to console output buffer (true) or a redirected output stream (false).
        bool bIsConsoleOutput = true;

        /// Saved console input mode from prior to instantiating the console.
        DWORD prevConsoleInputMode = 0;

        /// Saved console output mode from prior to instantiating the console.
        DWORD prevConsoleOutputMode = 0;
    };
}