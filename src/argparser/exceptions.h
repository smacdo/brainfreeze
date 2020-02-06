// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include "argparser.h"

namespace Brainfreeze::ArgParsing
{
    class OptionConfigurationException : public ArgParserException
    {
    public:
        OptionConfigurationException(
            const std::string& optionName,
            const std::string& errorMessage,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(const std::string& optionName, const std::string& errorMessage);
    };

    class UnknownOptionNameException : public ArgParserException
    {
    public:
        explicit UnknownOptionNameException(
            const std::string& optionName,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(const std::string& optionName);
    };

    class UnknownLongNameException : public ArgParserException
    {
    public:
        explicit UnknownLongNameException(
            const std::string& longName,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(const std::string& longName);
    };

    class UnknownShortNameException : public ArgParserException
    {
    public:
        explicit UnknownShortNameException(
            char shortName,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(char shortName);
    };

    class ExpectedArgumentMissingException : public ArgParserException
    {
    public:
        explicit ExpectedArgumentMissingException(
            const std::string& optionName,
            size_t argumentIndex,
            size_t totalExpectedArguments,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(
            const std::string& longName,
            size_t argumentIndex,
            size_t totalExpectedArguments);
    };
}