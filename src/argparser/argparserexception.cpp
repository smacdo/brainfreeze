// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include <sstream>

using namespace Brainfreeze;

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
UnknownParameterLongNameException::UnknownParameterLongNameException(
        const std::string& longName,
        const char* fileName,
        size_t lineNumber)
    : ArgParserException(format(longName), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string UnknownParameterLongNameException::format(const std::string& longName)
{
    std::stringstream ss;
    ss << "Unrecogonized parameter '" << longName << "'";
    return ss.str();
}

//=====================================================================================================================
ExpectedParameterArgumentMissingException::ExpectedParameterArgumentMissingException(
        const std::string& parameterName,
        size_t argumentIndex,
        size_t totalExpectedArguments,
        const char* fileName,
        size_t lineNumber)
    : ArgParserException(format(parameterName, argumentIndex, totalExpectedArguments), fileName, lineNumber)
{
}

//---------------------------------------------------------------------------------------------------------------------
std::string ExpectedParameterArgumentMissingException::format(
    const std::string& parameterName,
    size_t argumentIndex,
    size_t totalExpectedArguments)
{
    std::stringstream ss;

    if (totalExpectedArguments > 1)
    {
        ss << "Parameter '" << parameterName << "' missing argument #" << argumentIndex + 1 << "/"
           << totalExpectedArguments;
    }
    else
    {
        ss << "Parameter '" << parameterName << "' missing expected argument" << std::endl;
    }

    return ss.str();
}