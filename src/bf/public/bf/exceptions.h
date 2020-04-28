// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include <stdexcept>
#include <string>

namespace Brainfreeze
{
    /** Holds information on an error encountered while compiling user provided source code. */
    class CompileException : public std::runtime_error
    {
    public:
        /** Constructor. */
        CompileException(
            const std::string& errorMessage,        ///< Actual error message.
            size_t charOffset,                      ///< Offset from start of program text where error happened.
            int line,                               ///< Line number where error happened.
            int column)                             ///< Column number where error happened.
            : std::runtime_error(errorMessage),
              charOffset_(charOffset),
              line_(line),
              column_(column)
        {
        }

        /** Get the offset from the start of program text of the first character involved in the exxception. */
        const size_t charOffset() const noexcept { return charOffset_; }

        /** Get the line number where the error happened. */
        const int lineNumber() const noexcept { return line_; }

        /** Get the column number where the error happened. */
        const int columnNumber() const noexcept { return column_; }

    private:
        size_t charOffset_ = 0;
        int line_ = 0;
        int column_ = 0;
    };
}