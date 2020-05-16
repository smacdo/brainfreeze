// Copyright 2009-2020, Scott MacDonald.
#pragma once

#include <cstdint>

namespace Brainfreeze
{
    /** Abstracts console window handling from the interpreter. */
    class IConsole
    {
    public:
        /** Default constructor. */
        IConsole() = default;

        /** Destructor. */
        virtual ~IConsole();

        /** Get if characters should be echoed when typed. */
        bool shouldEchoCharForInput() const noexcept { return bEchoCharWhenReading_; }

        /** Set if characters should be echoed when typed. */
        void setShouldEchoCharForInput(bool newValue) noexcept { bEchoCharWhenReading_ = newValue; }

        /** Get if CR (\r and \r\n) should be converted to LF (\n) when read from input. */
        bool shouldConvertInputCRtoLF() const noexcept { return bConvertInputCRtoLF_; }

        /** Set if CR (\r and \r\n) should be converted to LF (\n) when read from input. */
        void setShouldConvertInputCRtoLF(bool newValue) noexcept { bConvertInputCRtoLF_ = newValue; }

        /** Get if LF (\n) should be converted to CRLF (\r\n) when written to output. */
        bool shouldConvertOutputLFtoCRLF() const noexcept { return bConvertOutputLFtoCRLF_; }

        /** Set if LF (\n) should be converted to CRLF (\r\n) when written to output. */
        void setShouldConvertOutputLFtoCRLF(bool newValue) noexcept { bConvertOutputLFtoCRLF_ = newValue; }

        /** Default write implementation: writes a byte to standard output. */
        virtual void write(char d) = 0;

        /** Default read implementation: reads a byte from standard input. */
        virtual char read() = 0;

        IConsole(const IConsole&) = delete;
        IConsole& operator =(const IConsole&) = delete;

    private:
        bool bEchoCharWhenReading_ = false;
        bool bConvertInputCRtoLF_ = true;
        bool bConvertOutputLFtoCRLF_ = true;
    };
}
