// Copyright 2009-2020, Scott MacDonald.
#include "windows_console.h"
#include "bf/bf.h"

#include <loguru/loguru.hpp>
#include <array>
#include <stdexcept>
#include <cassert>

#include <Windows.h>
#include <iostream> // TODO: REMOVE

using namespace Brainfreeze;
using namespace Brainfreeze::CommandLineApp;

// TODO: ReadFile is not sufficient for console input, it does not return until enter is pushed!
//---------------------------------------------------------------------------------------------------------------------
namespace
{
    struct ansi_color_info_t
    {
        ansi_color_info_t(const char* name, WORD foreground, WORD background)
            : name(name), foreground(foreground), background(background)
        {
        }

        const char* name;
        WORD foreground;
        WORD background;
    };

    /** Table of ansi colors mapped to their Windows foreground and background character attribute values. */
    std::array<ansi_color_info_t, AnsiColorCount> GAnsiColorTable
    {
        ansi_color_info_t { "Black",         0, 0  << 4 },
        ansi_color_info_t { "DarkRed",       4, 4  << 4 },
        ansi_color_info_t { "DarkGreen",     2, 2  << 4 },
        ansi_color_info_t { "DarkYellow",    6, 6  << 4 },
        ansi_color_info_t { "DarkBlue",      1, 1  << 4 },
        ansi_color_info_t { "DarkMagenta",   5, 5  << 4 },
        ansi_color_info_t { "DarkCyan",      3, 3  << 4 },
        ansi_color_info_t { "BrightGray",    7, 7  << 4 },
        ansi_color_info_t { "DarkGray",      8, 8  << 4 },
        ansi_color_info_t { "LightRed",     12, 12 << 4 },
        ansi_color_info_t { "LightGreen",   10, 10 << 4 },
        ansi_color_info_t { "LightYellow",  14, 14 << 4 },
        ansi_color_info_t { "LightBlue",     9, 9  << 4 },
        ansi_color_info_t { "LightMagenta", 13, 13 << 4 },
        ansi_color_info_t { "LightCyan",    11, 11 << 4 },
        ansi_color_info_t { "White",        15, 15 << 4 }
    };

    /** Takes a Windows character attribute word value, extracts the foreground color and converts it to AnsiColor. */
    constexpr AnsiColor ExtractForegroundColor(WORD charAttributes)
    {
        auto foregroundColor = (charAttributes & 15);

        for (size_t i = 0; i < GAnsiColorTable.size(); ++i)
        {
            if (GAnsiColorTable[i].foreground == foregroundColor)
            {
                return static_cast<AnsiColor>(i);
            }
        }

        throw std::runtime_error("Failed to find match for foreground color in lookup table");
    }

    /** Takes a Windows character attribute word value, extracts the background color and converts it to AnsiColor. */
    constexpr AnsiColor ExtractBackgroundColor(WORD charAttributes)
    {
        auto backgroundColor = (charAttributes & (15 << 4));

        for (size_t i = 0; i < GAnsiColorTable.size(); ++i)
        {
            if (GAnsiColorTable[i].background == backgroundColor)
            {
                return static_cast<AnsiColor>(i);
            }
        }

        throw std::runtime_error("Failed to find match for background color in lookup table");
    }
}

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
    bIsConsoleInput_ = (GetFileType(inputHandle_) == FILE_TYPE_CHAR);
    bIsConsoleOutput_ = (GetFileType(outputHandle_) == FILE_TYPE_CHAR);

    if (bIsConsoleInput_ && !GetConsoleMode(inputHandle_, &prevConsoleInputMode_))
    {
        throw std::runtime_error("Failed to save console input mode");
    }

    if (bIsConsoleOutput_ && !GetConsoleMode(outputHandle_, &prevConsoleOutputMode_))
    {
        throw std::runtime_error("Failed to save console output mode");
    }

    // Save the current console output character attributes if output is not redirected.
    if (bIsConsoleOutput_)
    {
        CONSOLE_SCREEN_BUFFER_INFO screenInfo;

        if (!GetConsoleScreenBufferInfo(outputHandle_, &screenInfo))
        {
            throw std::runtime_error("Failed to save output character attributes");
        }

        prevConsoleOutputCharAttributes_ = screenInfo.wAttributes;

        // Set current text foreground and background colors.
        currentTextForegroundColor_ = ExtractForegroundColor(prevConsoleOutputCharAttributes_);
        currentTextBackgroundColor_ = ExtractBackgroundColor(prevConsoleOutputCharAttributes_);
    }

    // Disable line input mode when we are attached to console input. This flag tells ReadFile to block until a
    // carriage return is read which prevents us from getting single character values.
    if (bIsConsoleInput_)
    {
        DWORD newInputMode = prevConsoleInputMode_ & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT;

        if (!SetConsoleMode(inputHandle_, newInputMode))
        {
            // TODO: Throw all exceptions with an error message like this.
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
    if (bIsConsoleInput_ && inputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(inputHandle_, prevConsoleInputMode_);
    }

    if (bIsConsoleOutput_ && outputHandle_ != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(outputHandle_, prevConsoleOutputMode_);
        SetConsoleTextAttribute(outputHandle_, prevConsoleOutputCharAttributes_);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::write(char d)
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
char WindowsConsole::read()
{
    // Indicate to user that Brainfreeze is waiting for their input.
    if (!isInputRedirected())
    {
        SetConsoleTitleA("Brainfreeze (waiting for input...)");
    }
    
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
        write(c);
    }

    // Restore old title.
    if (!isInputRedirected())
    {
        setTitle(windowTitle_);
    }

    // TODO: Remove all these log statements.
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
    } while ((waitForCharacter || (bIsConsoleInput_ && waitForCharacter) && inputBuffer.empty()));

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

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isInputRedirected() const
{
    return !bIsConsoleInput_;
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isOutputRedirected() const
{
    return !bIsConsoleOutput_;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTextColor(AnsiColor foreground, AnsiColor background)
{
    if (!isOutputRedirected())
    {
        assert(outputHandle_ != INVALID_HANDLE_VALUE);

        auto color = GAnsiColorTable[(int)foreground].foreground | GAnsiColorTable[(int)background].background;
        SetConsoleTextAttribute(outputHandle_, (WORD)color);

        currentTextForegroundColor_ = foreground;
        currentTextBackgroundColor_ = background;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTextForegroundColor(AnsiColor color)
{
    setTextColor(color, currentTextBackgroundColor_);
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTextBackgroundColor(AnsiColor color)
{
    setTextColor(currentTextForegroundColor_, color);
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::resetTextColors()
{
    resetTextForegroundColor();
    resetTextBackgroundColor();
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::resetTextForegroundColor()
{
    setTextForegroundColor(ExtractForegroundColor(prevConsoleOutputCharAttributes_));
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::resetTextBackgroundColor()
{
    setTextBackgroundColor(ExtractBackgroundColor(prevConsoleOutputCharAttributes_));
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTitle(std::string_view title)
{
    SetConsoleTitleA(title.data());
    windowTitle_ = title;
}
