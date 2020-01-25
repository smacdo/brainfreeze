// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include "exceptions.h"

#include <string>
#include <stdexcept>
#include <cassert>

using namespace Brainfreeze::ArgParsing;

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
// TODO: Allow/disallow parameter being specified more than once.

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
            // Remove the two front dashes before parsing it as a long option name.
            argument.erase(0, 2);
            parseLongName(argument);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::parseLongName(const std::string& longName)
{
    auto& option = findOptionByLongName(longName);

    // TODO: Handle if this option was specified multiple times.
    // Mark the parameter as being present.
    option.markSet(true);

    // Invoke any callbacks associated with parsing this option.
    option.invokeOnParsed();

    // TODO: Make sure this is a valid option argument (no short/long names).

    // Read expected arguments from the remaining unparsed arguments.
    // TODO: Handle optional arguments.
    while (option.expectsMoreArguments())
    {
        // Make sure there is another argument to read.
        if (nextArgIndex_ >= argsToParse_.size())
        {
            throw ExpectedArgumentMissingException(
                longName,
                option.argumentCount(),
                option.desc().expectedArgumentCount(),
                __FILE__, __LINE__);
        }

        // Add the next argument to the current option.
        // TODO: Check the next argument is "valid" (eg doesn't have - or --, etc)
        // TODO: Handle strings (is that here or somewhere else?)
        option.addArgument(argsToParse_[nextArgIndex_++]);
    }

    // TODO: Handle action associated with argument.
    // TODO: Handle parameters associated with argument.
    // TODO: Handle empty argument name.
    // TODO: Handle arguments not found.
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder ArgParser::addOption(const std::string& optionName)
{
    return OptionBuilder(optionName, optionName, *this);
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::registerOption(const std::string& name, const OptionDesc& desc)
{
    // TODO: Throw exception if name already added.
    assert(options_.find(name) == options_.end());
    assert(desc.hasLongName());

    // Add the option and then register it in look up tables.
    options_[name] = std::make_unique<OptionState>(desc);

    if (desc.hasShortName())
    {
        // TODO: Throw an exception.
        assert(shortNameToOptionLUT_.find(desc.shortName()) == shortNameToOptionLUT_.end());
        shortNameToOptionLUT_[desc.shortName()] = name;
    }

    if (desc.hasLongName())
    {
        // TODO: Throw an exception.
        assert(longNameToOptionLUT_.find(desc.longName()) == longNameToOptionLUT_.end());
        longNameToOptionLUT_[desc.longName()] = name;
    }
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

//---------------------------------------------------------------------------------------------------------------------
const OptionState& ArgParser::findOption(const std::string& optionName) const
{
    const OptionState* optionState = nullptr;

    if (!tryFindOption(optionName, &optionState))
    {
        throw UnknownOptionNameException(optionName, __FILE__, __LINE__);
    }

    assert(optionState != nullptr);
    return *optionState;
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOption(
    const std::string& optionName,
    const OptionState** optionState) const
{
    auto optionItr = options_.find(optionName);

    if (optionItr != options_.end())
    {
        if (optionState != nullptr)
        {
            *optionState = optionItr->second.get();
        }

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOption(
    const std::string& optionName,
    OptionState** optionState)
{
    auto optionItr = options_.find(optionName);

    if (optionItr != options_.end())
    {
        if (optionState != nullptr)
        {
            *optionState = optionItr->second.get();
        }

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOptionByLongName(
    const std::string& longName,
    const OptionState** optionState) const
{
    auto optNameItr = longNameToOptionLUT_.find(longName);

    if (optNameItr != longNameToOptionLUT_.end())
    {
        auto optionName = optNameItr->second;
        return tryFindOption(optionName, optionState);
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOptionByLongName(
    const std::string& longName,
    OptionState** optionState)
{
    auto optNameItr = longNameToOptionLUT_.find(longName);

    if (optNameItr != longNameToOptionLUT_.end())
    {
        auto optionName = optNameItr->second;
        return tryFindOption(optionName, optionState);
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
const OptionState& ArgParser::findOptionByLongName(const std::string& longName) const
{
    const OptionState* optionState = nullptr;

    if (!tryFindOptionByLongName(longName, &optionState))
    {
        throw UnknownLongNameException(longName, __FILE__, __LINE__);
    }

    assert(optionState != nullptr);
    return *optionState;
}

//---------------------------------------------------------------------------------------------------------------------
OptionState& ArgParser::findOptionByLongName(const std::string& longName)
{
    OptionState* optionState = nullptr;

    if (!tryFindOptionByLongName(longName, &optionState))
    {
        throw UnknownLongNameException(longName, __FILE__, __LINE__);
    }

    assert(optionState != nullptr);
    return *optionState;
}
