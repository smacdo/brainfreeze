// Copyright 2009-2020, Scott MacDonald.
#include "windows_console.h"
#include "bf/bf.h"

#include <array>
#include <stdexcept>
#include <cassert>
#include <sstream>

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
    : bufferedChars_()
{
    initStream(&inputStream_, GetStdHandle(STD_INPUT_HANDLE), false);
    initStream(&outputStream_, GetStdHandle(STD_OUTPUT_HANDLE), true);
    initStream(&errorStream_, GetStdHandle(STD_ERROR_HANDLE), true);

    // Get current text colors.
    if (outputStream_.isConsole || errorStream_.isConsole)
    {
        auto attr = (outputStream_.isConsole ? outputStream_.outputCharAttributes : errorStream_.outputCharAttributes);
        currentTextForegroundColor_ = ExtractForegroundColor(attr);
        currentTextBackgroundColor_ = ExtractBackgroundColor(attr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
WindowsConsole::~WindowsConsole()
{
    restoreStreamPrevState(inputStream_);
    restoreStreamPrevState(outputStream_);
    restoreStreamPrevState(errorStream_);
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::write(char d, OutputStreamName streamName)
{
    auto& stream = (streamName == OutputStreamName::Stdout ? outputStream_ : errorStream_);
    auto handle = stream.handle;

    std::array<char, 2> outBuffer = { d, '\0' };
    
    if (shouldConvertOutputLFtoCRLF() && d == '\n')
    {
        outBuffer[0] = '\r';
        outBuffer[1] = '\n';

        WriteFile(handle, outBuffer.data(), 2, nullptr, nullptr);
    }
    else
    {
        WriteFile(handle, outBuffer.data(), 1, nullptr, nullptr);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::write(std::string_view, OutputStreamName)
{
    // TODO: support LF -> CRLF option for newlines found in message.
    throw std::runtime_error("TODO: Implement me");
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::writeLine(std::string_view, OutputStreamName)
{
    throw std::runtime_error("TODO: Implement me");
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
    readBuffered();

    // Exit early with a configurable end of stream value if there are no more characters left in the input stream.
    if (bufferedChars_.empty())
    {
        // TODO: Allow configurable return value.
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
        }
        
        c = '\n';
    }

    // Echo character if requested.
    if (shouldEchoCharForInput())
    {
        write(c, OutputStreamName::Stdout);
    }

    // Restore old title.
    if (!isInputRedirected())
    {
        setTitle(windowTitle_);
    }

    // TODO: Remove all these log statements.
    return c;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::readBuffered(bool waitForCharacter)
{
    assert(inputStream_.handle != INVALID_STREAM_HANDLE);

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

    do 
    {
        if (!ReadFile(inputStream_.handle, inputBuffer.data(), (DWORD)inputBuffer.size(), &numBytesRead, nullptr))
        {
            throw std::runtime_error("Failed to read input; ReadConsoleInput returned false");
        }
    } while ((waitForCharacter || (inputStream_.isConsole && waitForCharacter) && inputBuffer.empty()));

    // Copy all characters into the character buffer.
    assert(numBytesRead <= inputBuffer.size());

    for (DWORD i = 0; i < numBytesRead; ++i)
    {
        bufferedChars_.push(inputBuffer[i]);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::clearInputBuffer()
{
    while (!bufferedChars_.empty())
    {
        bufferedChars_.pop();
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isInputRedirected() const
{
    return !inputStream_.isConsole;
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isOutputRedirected() const
{
    return !outputStream_.isConsole;
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isErrorRedirected() const
{
    throw !errorStream_.isConsole;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTextColor(AnsiColor foreground, AnsiColor background)
{
    if (outputStream_.isConsole || errorStream_.isConsole)
    {
        auto& stream = (outputStream_.isConsole ? outputStream_ : errorStream_);
        auto handle = stream.handle;

        assert(handle != INVALID_HANDLE_VALUE);

        auto color = GAnsiColorTable[(int)foreground].foreground | GAnsiColorTable[(int)background].background;
        SetConsoleTextAttribute(handle, (WORD)color);

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
void WindowsConsole::setTextFormat(AnsiFormatOption, bool)
{
    throw std::runtime_error("TODO: Implement me");
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::resetTextFormatting()
{
    throw std::runtime_error("TODO: Implement me");
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
    if (outputStream_.isConsole || errorStream_.isConsole)
    {
        auto& stream = (outputStream_.isConsole ? outputStream_ : errorStream_);
        setTextForegroundColor(ExtractForegroundColor(stream.outputCharAttributes));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::resetTextBackgroundColor()
{
    if (outputStream_.isConsole || errorStream_.isConsole)
    {
        auto& stream = (outputStream_.isConsole ? outputStream_ : errorStream_);
        setTextBackgroundColor(ExtractBackgroundColor(stream.outputCharAttributes));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setTitle(std::string_view title)
{
    SetConsoleTitleA(title.data());
    windowTitle_ = title;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setInputBuffering(bool isEnabled)
{
    // Do not apply any terminal changes if input was redirected.
    if (isInputRedirected())
    {
        return;
    }

    // Get the current input mode to change it.
    DWORD inputMode = 0;

    if (!GetConsoleMode(inputStream_.handle, &inputMode))
    {
        raiseError(GetLastError(), "Failed to get console input mode", __FILE__, __LINE__);
        return;
    }

    // Add or remove the ENABLE_LINE_INPUT flag from the input mode.
    if (isEnabled)
    {
        inputMode |= ENABLE_LINE_INPUT;
    }
    else
    {
        inputMode &= ~ENABLE_LINE_INPUT;
    }

    // Now apply the input mode.
    if (!SetConsoleMode(inputStream_.handle, inputMode))
    {
        raiseError(GetLastError(), "Failed to set console input mode", __FILE__, __LINE__);
        return;
    }

    bIsInputBuffered_ = isEnabled;
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isInputBufferingEnabled() const noexcept
{
    return bIsInputBuffered_;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::setInputEchoing(bool isEnabled)
{
    IConsole::setShouldEchoCharForInput(isEnabled);

    // Do not apply any terminal changes if input was redirected.
    if (isInputRedirected())
    {
        return;
    }

    // Get the current input mode to change it.
    DWORD inputMode = 0;

    if (!GetConsoleMode(inputStream_.handle, &inputMode))
    {
        raiseError(GetLastError(), "Failed to get console input mode", __FILE__, __LINE__);
        return;
    }

    // Add or remove the ENABLE_LINE_INPUT flag from the input mode.
    if (isEnabled)
    {
        inputMode |= ENABLE_ECHO_INPUT;
    }
    else
    {
        inputMode &= ~ENABLE_ECHO_INPUT;
    }

    // Now apply the input mode.
    if (!SetConsoleMode(inputStream_.handle, inputMode))
    {
        raiseError(GetLastError(), "Failed to set console input mode", __FILE__, __LINE__);
        return;
    }

    bIsInputEchoed_ = isEnabled;
}

//---------------------------------------------------------------------------------------------------------------------
bool WindowsConsole::isInputEchoingEnabled() const noexcept
{
    return bIsInputEchoed_;
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::raiseError(DWORD error, const char* action, const char* filename, int lineNumber)
{
    if (error == 0)
    {
        // TODO: HANDLE
    }

    // Get the error message as a string.
    // Ref: https://stackoverflow.com/a/17387176/1922926
    LPSTR messageBuffer = nullptr;

    auto size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0,                                              // Location of message definition.
        error,                                          // Message id.
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),      // language id.
        (LPSTR)&messageBuffer,                          // Pointer to buffer that receives the message.
        0,                                              // FormatMessageA should allocate the buffer.
        nullptr);                                       // No formatting arguments.

    std::string errorMessage(messageBuffer, size);
    LocalFree(messageBuffer);

    // Create a formatted error message.
    std::stringstream ss;
    ss << "[CONSOLE] " << action << ": " << errorMessage << " in " << filename << ":" << lineNumber << std::endl;
    
    // Print the formatted message to the console.
    // TODO: Print as bold red text.
    // TODO: Make sure not INVALID_HANDLE
    auto formattedMessage = ss.str();
    WriteFile(errorStream_.handle, formattedMessage.data(), (DWORD)formattedMessage.length(), nullptr, nullptr);
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::initStream(stream_data_t* streamData, HANDLE standardHandle, bool isOutputHandle)
{
    assert(streamData != nullptr);

    // Check that input and output handles are valid.
    if (standardHandle == INVALID_HANDLE_VALUE)
    {
        raiseError(0, "Initial standard handle is not valid", __FILE__, __LINE__);
        return;
    }

    // Save the current console mode (and restore when finished) but only if input / output is a terminal. Otherwise
    // either input or output is redirected and should not be treated as a console terminal.
    streamData->handle = standardHandle;
    streamData->isOutput = isOutputHandle;
    streamData->isConsole = (GetFileType(standardHandle) == FILE_TYPE_CHAR);

    if (streamData->isConsole && !GetConsoleMode(standardHandle, &(streamData->prevMode)))
    {
        raiseError(GetLastError(), "Failed to save console mode", __FILE__, __LINE__);
    }

    // Save the current console output character attributes if output is not redirected.
    if (isOutputHandle && streamData->isConsole)
    {
        CONSOLE_SCREEN_BUFFER_INFO screenInfo;

        if (GetConsoleScreenBufferInfo(standardHandle, &screenInfo))
        {
            streamData->outputCharAttributes = screenInfo.wAttributes;
        }
        else
        {
            raiseError(GetLastError(), "Failed to save console character attributes", __FILE__, __LINE__);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsConsole::restoreStreamPrevState(const stream_data_t& stream)
{
    if (stream.handle != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(stream.handle, stream.prevMode);

        if (stream.isOutput)
        {
            SetConsoleTextAttribute(stream.handle, stream.outputCharAttributes);
        }
    }
}
