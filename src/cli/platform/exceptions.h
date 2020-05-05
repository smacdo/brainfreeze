// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "exception.h"

namespace Brainfreeze::CommandLineApp
{
    /** Holds error information generated by a POSIX error code. */
    class POSIXException : public PlatformException
    {
    public:
        /** Constructor. */
        POSIXException(
            int errorCode,                      ///< POSIX error code.
            const char* sourceFileName,         ///< Name of the source code file that generated the exception.
            int sourceLineNumber);              ///< Line number of the source code file that generated the exception.
        
        /** Get the POSIX error code associated with this exception. */
        int errorCode() const noexcept;

        /** Get error message associated with error code. */
        static std::string getMessageForError(int errorCode);

    private:
        int errorCode_ = 0;
    };
}