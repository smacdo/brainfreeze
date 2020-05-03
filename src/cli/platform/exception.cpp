// Copyright 2009-2020, Scott MacDonald.
#include "exception.h"

#include <sstream>

using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
PlatformException::PlatformException(
    std::string message,
    const char* sourceFileName,
    int sourceLineNumber)
        : std::runtime_error(format(message, sourceFileName, sourceLineNumber)),
          originalMessage_(std::move(message)),
          sourceFileName_(sourceFileName),
          sourceLineNumber_(sourceLineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
PlatformException::~PlatformException() = default;

//---------------------------------------------------------------------------------------------------------------------
std::string PlatformException::format(
    std::string_view message,
    const char* sourceFileName,
    int sourceLineNumber)
{
    if (sourceFileName != nullptr)
    {
        return std::string(message.data(), message.length());
    }
    else
    {
        std::stringstream ss;
        ss << message;
        ss << " (" << sourceFileName;

        if (sourceLineNumber > 0)
        {
            ss << ":" << sourceLineNumber;
        }

        ss << ")";

        return ss.str();
    }
}

//---------------------------------------------------------------------------------------------------------------------
const std::string& PlatformException::originalMessage() const noexcept
{
    return originalMessage_;
}

//---------------------------------------------------------------------------------------------------------------------
bool PlatformException::hasSourceFileName() const noexcept
{
    return sourceFileName_ != nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
bool PlatformException::hasSourceLineNumber() const noexcept
{
    return sourceLineNumber_ > 0;
}

//---------------------------------------------------------------------------------------------------------------------
std::string_view PlatformException::sourceFileName() const noexcept
{
    return sourceFileName_;
}

//---------------------------------------------------------------------------------------------------------------------
int PlatformException::sourceLineNumber() const noexcept
{
    return sourceLineNumber_;
}
