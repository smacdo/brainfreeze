// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "bf.h"
#include <Windows.h>
#include <queue>

namespace Brainfreeze
{
    /** Windows specific console implementation. */
    class WindowsConsole : public IConsole
    {
    public:
        WindowsConsole();
        virtual ~WindowsConsole();

        /** Write a byte to the Windows console. */
        virtual void Write(Interpreter::byte_t d) override;

        /** Read a byte from the Windows console. */
        virtual Interpreter::byte_t Read() override;

    private:
        void RefillInputBuffer(bool waitForLegitChar = true);
        void ClearBufferedInput();

    private:
        static const size_t DefaultBufferSize = 256;
        std::queue<char> bufferedChars_;
        bool echoCharWhenReading = true;
        bool convertInputCRtoLF = true;
        bool convertOutputLFtoCRLF = true;
        HANDLE stdInputHandle_ = INVALID_HANDLE_VALUE;
        HANDLE stdOutputHandle_ = INVALID_HANDLE_VALUE;
        DWORD prevConsoleInputMode = 0;
        DWORD prevConsoleOutputMode = 0;
    };
}