// Copyright 2009-2020, Scott MacDonald.
#include "windows_console.h"
#include "bf.h"

#include <loguru/loguru.hpp>
#include <array>
#include <stdexcept>
#include <cassert>

#include <Windows.h>

using namespace Brainfreeze;
// TODO: ReadFile is not sufficient for console input, it does not return until enter is pushed!

//---------------------------------------------------------------------------------------------------------------------
WindowsConsole::WindowsConsole()
    : bufferedChars_(),
      inputHandle_(GetStdHandle(STD_INPUT_HANDLE)),
      outputHandle_(GetStdHandle(STD_OUTPUT_HANDLE))
{
    // Check that input and output handles are valid.
    if (inputHandle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to get standard input handle for console i/o");
    }

    if (outputHandle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to get standard output handle for console i/o");
    }

    // Save the current console mode (and restore when finished) but only if input / output is a terminal. Otherwise
    // either input or output is redirected and should not be treated as a console terminal.
    bIsConsoleInput = (GetFileType(inputHandle_) == FILE_TYPE_CHAR);
    bIsConsoleOutput = (GetFileType(outputHandle_) == FILE_TYPE_CHAR);

    if (bIsConsoleInput && !GetConsoleMode(inputHandle_, &prevConsoleInputMode))
    {
        throw std::runtime_error("Failed to save console input mode");
    }

    if (bIsConsoleOutput && !GetConsoleMode(outputHandle_, &prevConsoleOutputMode))
    {
        throw std::runtime_error("Failed to save console output mode");
    }

    // Disable line input mode when we are attached to console input. This flag tells ReadFile to block until a
    // carriage return is read which prevents us from getting single character values.
    if (bIsConsoleInput)
    {
        DWORD newInputMode = prevConsoleInputMode & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT;

        if (!SetConsoleMode(inputHandle_, newInputMode))
        {
            std::string message = "Failed to disable ENABLE_LINE_INPUT for console input";
            message += std::to_string(GetLastError());

            throw std::runtime_error(message);
        }

        LOG_F(1, "Disabled console input buffered line read mode");
    }
}

//---------------------------------------------------------------------------------------------------------------------
WindowsConsole::~WindowsConsole()
{
    // Restore potentially modified console i/o modes.
    if (bIsConsoleInput && inputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(inputHandle_, prevConsoleInputMode);
    }

    if (bIsConsoleOutput && outputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(outputHandle_, prevConsoleOutputMode);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::Write(char d)
{
    LOG_F(1, "Writing char %d '%c'", (int)d, (char)d);

    std::array<char, 2> outBuffer = { d, '\0' };
    
    if (shouldConvertOutputLFtoCRLF() && d == '\n')
    {
        LOG_F(1, "Replacing \\n with \\r\\n and then writing char");
        outBuffer[0] = '\r';
        outBuffer[1] = '\n';

        WriteFile(outputHandle_, outBuffer.data(), 2, nullptr, nullptr);
    }
    else
    {
        WriteFile(outputHandle_, outBuffer.data(), 1, nullptr, nullptr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
char WindowsConsole::Read()
{
    // Acquire more characters from the input stream.
    LOG_F(1, "BF read requested");
    ReadBuffered();

    // Exit early with a configurable end of stream value if there are no more characters left in the input stream.
    if (bufferedChars_.empty())
    {
        // TODO: Allow configurable return value.
        LOG_F(2, "End of stream, returning %d", (int)0);
        return EOF;
    }

    // Get the next unconsumed input character.
    char c = static_cast<Interpreter::byte_t>(bufferedChars_.front());
    bufferedChars_.pop();

    // Perform newline conversion if requested.
    if (shouldConvertInputCRtoLF() && c == '\r')
    {
        if (!bufferedChars_.empty() && bufferedChars_.front() == '\n')
        {
            bufferedChars_.pop();
            LOG_F(1, "Replacing CRLF \\r\\n with LF (\\n)");
        }
        else
        {
            LOG_F(1, "Replacing CR (\\r) with LF (\\n)");
        }
        
        c = '\n';
    }

    // Echo character if requested.
    if (shouldEchoCharForInput())
    {
        Write(c);
    }

    LOG_F(2, "Returning char %d '%c'", (int)c, (char)c);
    return c;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::ReadBuffered(bool waitForCharacter)
{
    // If there are unread characters in the input buffer exit early to avoid extra work to read more characters. It is
    // more efficient to batch work by waiting until the input buffer is drained and then reading as many characters
    // (up to the buffer size) as possible in one go.
    if (!bufferedChars_.empty())
    {
        return;
    }

    // Batch read as many bytes as there are available in the input queue. 
    //
    // Where are the input bytes coming from? If the input being read is from the actual console input buffer we
    // need to wait for the user to press at least one key. Additionally there is no concept of 'end of stream' since
    // the user can always type more.
    //
    // If the input is a redirected input stream then different considerations apply. Input should always be available
    // but at some point we will hit the end of the stream and need to stop reading more bytes. It is important in this
    // mode NOT to loop and wait for more characters.
    std::array<char, DefaultBufferSize> inputBuffer;
    DWORD numBytesRead = 0;

    LOG_F(2, "Buffered input array is empty, trying to fill");

    do 
    {
        if (!ReadFile(inputHandle_, inputBuffer.data(), (DWORD)inputBuffer.size(), &numBytesRead, nullptr))
        {
            throw std::runtime_error("Failed to read input; ReadConsoleInput returned false");
        }
    } while ((waitForCharacter || (bIsConsoleInput && waitForCharacter) && inputBuffer.empty()));

    // Copy all characters into the character buffer.
    assert(numBytesRead <= inputBuffer.size());

    for (DWORD i = 0; i < numBytesRead; ++i)
    {
        bufferedChars_.push(inputBuffer[i]);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::ClearInputBuffer()
{
    while (!bufferedChars_.empty())
    {
        bufferedChars_.pop();
    }
}
