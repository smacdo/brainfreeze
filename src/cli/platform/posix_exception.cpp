// Copyright 2009-2020, Scott MacDonald.
#include "exceptions.h"

#include <iostream>
#include <algorithm>
#include <string.h>

using namespace Brainfreeze::CommandLineApp;

//---------------------------------------------------------------------------------------------------------------------
POSIXException::POSIXException(
    int errorCode,
    const char* sourceFileName,
    int sourceLineNumber)
        : PlatformException(getMessageForError(errorCode), sourceFileName, sourceLineNumber),
          errorCode_(errorCode)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string POSIXException::getMessageForError(int errorCode)
{
    // Pass a presized string buffer to strerror_r to get the error message.
    constexpr const size_t kMaxLength = 256;
    std::string buffer(kMaxLength, ' ');
    buffer[kMaxLength - 1] = '\0';

#ifdef WIN32
    auto result = strerror_s(buffer.data(), kMaxLength, errorCode);
#else
    auto result = strerror_r(errorCode, buffer.data(), kMaxLength);
#endif

    // Trim the extraneous null terminators from the string so long as the call to strerror_r succeeded.
    auto endIndex = buffer.find_first_of('\0');

    if (result == 0)
    {
        buffer.resize(endIndex);
    }
    else if (endIndex == 0)
    {
        // The call to strerror_r failed (got error message) and didn't return any actual text. Put a generic error
        // message that can be returned to the caller.
        return "<POSIX ERROR - failed to get printable message from strerror_r>";
    }
    
    return buffer;
}

//---------------------------------------------------------------------------------------------------------------------
int POSIXException::errorCode() const noexcept
{
    return errorCode_;
}
