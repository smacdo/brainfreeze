// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"

#include <string>
#include <stdexcept>

using namespace Brainfreeze;

// TODO: Handle shortnames.
// TODO: Add parameter that can take all unbound arguments. Whenever the parser encounters an unbound argument (an
//       argument following a parameter or another argument) that does not belong to the previous parameter
//       (its expected count is exceeded and it does not have optional extra) this argument will be added to the most
//       recently encountered parameter that takes unbound arguments.
// TODO: Parameters can take extra arguments. This means any arguments immediately following this parameter will be
//       added to the parameter, even if its expected arg count is met.
// TODO: Add optional function callback when a parameter is found on the command line.
// TODO: Add optional function callback when a parameter argument is found. This can do validation.
// TODO: Add ability to specify if a flag/parameter can be encountered multiple times. This might be tricky.
// TODO: Support required options.
// TODO: Support explictly setting bool parameter value to true or false. Right now is always false unless the
//       parameter is passed in long or short form in which case it is always true.

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::parse(int argc, const char** argv)
{
    // TODO: Handle argv[0] being the program name which will break the unit tests.
    argsToParse_ = ArgcvToVector(argc, argv);
    nextArgIndex_ = 0;

    auto argIndex = nextArgIndex_;

    while (nextArgIndex_ < argsToParse_.size())
    {
        argIndex = nextArgIndex_++;

        // Read this argument as a string view.
        auto argument = argsToParse_[argIndex];

        // Is this the start of a short or long argument name?
        // TODO: Handle "--", "-", "---" cases
        if (argument.length() >= 2 && argument[0] == '-' && argument[1] == '-')
        {
            // Remove the two front dashes before parsing it as a long parameter name.
            argument.erase(0, 2);
            parseLongName(argument);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::parseLongName(const std::string& parameterName)
{
    // Find the parameter associated with this long name. If no argument is found throw an exception that an unknown
    // parameter was passed.
    auto paramItr = parameters_.find(parameterName);

    if (paramItr == parameters_.end())
    {
        throw UnknownParameterLongNameException(parameterName, __FILE__, __LINE__);
    }

    auto& param = paramItr->second;

    // Mark the parameter as being present.
    param.isSet = true;

    // Are there parameters associated with this?
    if (param.expectedArgumentCount > 0)
    {
        // Read as many of the expected arguments as possible.
        // TODO: Handle optional arguments.
        while (param.arguments.size() < param.expectedArgumentCount)
        {
            // Make sure there is another argument to read.
            if (nextArgIndex_ >= argsToParse_.size())
            {
                throw ExpectedParameterArgumentMissingException(
                    parameterName,
                    param.expectedArgumentCount - param.arguments.size() - 1,
                    param.expectedArgumentCount,
                    __FILE__, __LINE__);
            }

            // TODO: Make sure this is a valid parameter argument (no short/long names).
            param.arguments.push_back(argsToParse_[nextArgIndex_++]);
        }
    }

    // TODO: Handle action associated with argument.
    // TODO: Handle parameters associated with argument.
    // TODO: Handle empty argument name.
    // TODO: Handle arguments not found.
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::addFlag(
    const std::string& longName,
    const std::string& description)
{
    addFlag('\0', longName, description);
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::addFlag(
    char shortName,
    const std::string& longName,
    const std::string& description)
{
    // TODO: Handle duplicates.
    // TODO: Handle empty longName.
    parameter_t p;

    p.shortName = shortName;
    p.longName = longName;
    p.description = description;
    p.expectedArgumentCount = 0;

    parameters_[longName] = p; 
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::isFlagSet(const std::string& longName) const
{
    auto paramItr = parameters_.find(longName);

    if (paramItr != parameters_.end())
    {
        return paramItr->second.isSet;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::addParameter(
    const std::string& longName,
    size_t expectedArgumentCount,
    const std::string& description)
{
    addParameter('\0', longName, expectedArgumentCount, description);
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::addParameter(
    char shortName,
    const std::string& longName,
    size_t expectedArgumentCount,
    const std::string& description)
{
    parameter_t p;

    p.shortName = shortName;
    p.longName = longName;
    p.description = description;
    p.expectedArgumentCount = expectedArgumentCount;

    parameters_[longName] = p;
}

//---------------------------------------------------------------------------------------------------------------------
const std::vector<std::string>& ArgParser::getArgumentsForParameter(const std::string& parameterName) const
{
    auto paramItr = parameters_.find(parameterName);

    if (paramItr == parameters_.end())
    {
        throw std::runtime_error("Could not find a parameter with the name " + parameterName);
    }

    return paramItr->second.arguments;
}

//---------------------------------------------------------------------------------------------------------------------
std::vector<std::string> ArgParser::ArgcvToVector(int argc, const char** argv)
{
    // Can't parse a null array of arguments!
    if (argv == nullptr)
    {
        throw std::runtime_error("Cannot parse null arguments array");
    }

    // Parse all of the given argument blocks.
    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i)
    {
        if (argv[i] == nullptr)
        {
            throw std::runtime_error("Cannot parse null string in args arrau");
        }

        args.push_back(argv[i]);
    }

    return args;
}