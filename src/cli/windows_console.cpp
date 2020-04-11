// Copyright 2009-2020, Scott MacDonald.
#include "windows_console.h"
#include "bf.h"

#include <loguru/loguru.hpp>
#include <array>
#include <stdexcept>
#include <cassert>

#include <Windows.h>

using namespace Brainfreeze;

// TODO: store any unprocessed key events until a later read
// TODO: issue warning / error when program exits with multiple unread 

//---------------------------------------------------------------------------------------------------------------------
WindowsConsole::WindowsConsole()
    : bufferedChars_(),
      stdInputHandle_(GetStdHandle(STD_INPUT_HANDLE)),
      stdOutputHandle_(GetStdHandle(STD_OUTPUT_HANDLE))
{
    // Check that input and output handles are valid.
    if (stdInputHandle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to get standard input handle for console i/o");
    }

    if (stdOutputHandle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to get standard output handle for console i/o");
    }

    // Save console mode and restore it when the windows console instance is destroyed.
    if (!GetConsoleMode(stdInputHandle_, &prevConsoleInputMode))
    {
        throw std::runtime_error("Failed to save current console input mode");
    }

    if (!GetConsoleMode(stdOutputHandle_, &prevConsoleOutputMode))
    {
        throw std::runtime_error("Failed to save current console output mode");
    }

    // For some reason the input buffer contains extra characters like a carriage return even before a character is
    // typed. Clearing the input buffer as soon as the console is instantiated seems to work around this behavior.
    // TODO: Figure out why this behavior happens and if it can be properly handled.
    RefillInputBuffer(false);
    ClearBufferedInput();
}

//---------------------------------------------------------------------------------------------------------------------
WindowsConsole::~WindowsConsole()
{
    // Restore potentially modified console i/o modes.
    if (stdInputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(stdInputHandle_, prevConsoleInputMode);
    }

    if (stdOutputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(stdOutputHandle_, prevConsoleOutputMode);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::Write(Interpreter::byte_t d)
{
    LOG_F(1, "Writing char '%c' (%d)", (char)d, (int)d);

    std::array<char, 2> outBuffer = { d, '\0' };
    
    if (convertOutputLFtoCRLF && d == '\n')
    {
        LOG_F(1, "Replacing \\n with \\r\\n and then writing char");
        outBuffer[0] = '\r';
        outBuffer[1] = '\n';

        WriteConsole(stdOutputHandle_, outBuffer.data(), 2, nullptr, nullptr);
    }
    else
    {
        WriteConsole(stdOutputHandle_, outBuffer.data(), 1, nullptr, nullptr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::byte_t WindowsConsole::Read()
{
    LOG_F(1, "BF read requested");

    if (bufferedChars_.empty())
    {
        LOG_F(1, "Buffer empty, refilling");
        RefillInputBuffer();
    }

    assert(bufferedChars_.size() > 0);

    char c = static_cast<Interpreter::byte_t>(bufferedChars_.front());
    bufferedChars_.pop();

    if (convertInputCRtoLF && c == '\r')
    {
        LOG_F(1, "Replacing carriage return (\\r) with line feed (\\n)");
        c = '\n';
    }

    LOG_F(2, "Returning char '%c' (%d)", c, (int)c);

    if (echoCharWhenReading)
    {
        Write(c);
    }

    return c;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::RefillInputBuffer(bool waitForLegitChar)        // TODO: Better name.
{
    while (waitForLegitChar && bufferedChars_.size() == 0)
    {
        LOG_F(2, "Buffered array is empty, trying to fill");

        // Reads a single, unbuffered character from input without displaying it on the screen.
        std::array<INPUT_RECORD, DefaultBufferSize> inputEvents;
        DWORD numEventsRead = 0;

        // TODO: Throw exception if error returned!
        ReadConsoleInput(stdInputHandle_, inputEvents.data(), (DWORD)inputEvents.size(), &numEventsRead);
        LOG_F(2, "ReadConsoleInput returned %d events", (int)numEventsRead);

        // Copy all characters into the character buffer.
        for (DWORD i = 0; i < numEventsRead; ++i)
        {
            if (inputEvents[i].EventType != KEY_EVENT)
            {
                LOG_F(WARNING,
                    "Ignoring buffered console input event that is not of type KEY_EVENT (is %d)",
                    (int)inputEvents[i].EventType);
                continue;
            }

            if (inputEvents[i].Event.KeyEvent.bKeyDown)
            {
                LOG_F(2, "Get char %d '%c'", (int)inputEvents[i].Event.KeyEvent.uChar.AsciiChar, inputEvents[i].Event.KeyEvent.uChar.AsciiChar);
                bufferedChars_.push(inputEvents[i].Event.KeyEvent.uChar.AsciiChar);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::ClearBufferedInput()
{
    while (!bufferedChars_.empty())
    {
        bufferedChars_.pop();
    }
}
