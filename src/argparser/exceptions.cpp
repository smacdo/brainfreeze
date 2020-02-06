// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include "exceptions.h"
#include <sstream>

using namespace Brainfreeze::ArgParsing;

//---------------------------------------------------------------------------------------------------------------------
ArgParserException::ArgParserException(
        const std::string& message,
        const char* fileName,
        size_t lineNumber)
    : std::runtime_error(format(message, fileName, lineNumber)),
      message_(message),
      fileName_(fileName),
      lineNumber_(lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string ArgParserException::format(
    const std::string& message,
    const char* fileName,
    size_t lineNumber)
{
    std::stringstream ss;

    ss << message;

    if (fileName != nullptr)
    {
        ss << " (" << fileName;

        if (lineNumber > 0)
        {
            ss << ":" << lineNumber;
        }

        ss << ")";
    }

    return ss.str();
}

//=====================================================================================================================
OptionConfigurationException::OptionConfigurationException(
    const std::string& optionName,
    const std::string& errorMessage,
    const char* fileName,
    size_t lineNumber)
    : ArgParserException(format(optionName, errorMessage), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string OptionConfigurationException::format(const std::string& optionName, const std::string& errorMessage)
{
    std::stringstream ss;
    ss << "Invalid configuration for option '" << optionName << "': " << errorMessage;
    return ss.str();
}

//=====================================================================================================================
UnknownLongNameException::UnknownLongNameException(
        const std::string& longName,
        const char* fileName,
        size_t lineNumber)
    : ArgParserException(format(longName), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string UnknownLongNameException::format(const std::string& longName)
{
    std::stringstream ss;
    ss << "Unrecogonized option '" << longName << "'";
    return ss.str();
}

//=====================================================================================================================
UnknownShortNameException::UnknownShortNameException(
    char shortName,
    const char* fileName,
    size_t lineNumber)
    : ArgParserException(format(shortName), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string UnknownShortNameException::format(char shortName)
{
    std::stringstream ss;
    ss << "Unrecogonized option '" << shortName << "'";
    return ss.str();
}

//=====================================================================================================================
UnknownOptionNameException::UnknownOptionNameException(
    const std::string& optionName,
    const char* fileName,
    size_t lineNumber)
    : ArgParserException(format(optionName), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string UnknownOptionNameException::format(const std::string& optionName)
{
    std::stringstream ss;
    ss << "Unrecogonized option '" << optionName << "'";
    return ss.str();
}

//=====================================================================================================================
ExpectedArgumentMissingException::ExpectedArgumentMissingException(
        const std::string& optionName,
        size_t argumentIndex,
        size_t totalExpectedArguments,
        const char* fileName,
        size_t lineNumber)
    : ArgParserException(format(optionName, argumentIndex, totalExpectedArguments), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string ExpectedArgumentMissingException::format(
    const std::string& optionNAme,
    size_t argumentIndex,
    size_t totalExpectedArguments)
{
    std::stringstream ss;

    if (totalExpectedArguments > 1)
    {
        ss << "Option '" << optionNAme << "' missing argument #" << argumentIndex + 1 << "/"
           << totalExpectedArguments;
    }
    else
    {
        ss << "Option '" << optionNAme << "' missing expected argument" << std::endl;
    }

    return ss.str();
}