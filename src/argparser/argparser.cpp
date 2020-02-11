// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include "exceptions.h"

#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <algorithm>

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
std::unique_ptr<ArgParserResults> ArgParser::parse(
    int argc,
    const char** argv,
    bool extractProgramName)
{
    return parse(ArgcvToVector(argc, argv), extractProgramName);
}

//---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<ArgParserResults> ArgParser::parse(
    const std::vector<std::string>& args,
    bool extractProgramName)
{
    argsToParse_ = args;

    results_.reset(new ArgParserResults(options_));
    nextArgIndex_ = 0;
    nextPositionalOptionIndex_ = 0;

    positionalLUT_ = buildPositionalParameterLUT();

    // The first argument is always the name of the program that is running. Extract and save this value if it was
    // provided.
    // TODO: TEST
    if (argsToParse_.size() > 0 && extractProgramName)
    {
        results_->setProgramName(argsToParse_[0]);
        nextArgIndex_ = 1;
    }

    // Parse the remainder of the arguments.
    auto argIndex = nextArgIndex_;

    while (nextArgIndex_ < argsToParse_.size())
    {
        argIndex = nextArgIndex_++;

        // Read this argument as a string view.
        auto argument = argsToParse_[argIndex];

        // Is this the start of a short or long argument name?
        // TODO: Handle "--", "-", "---" cases.
        // TODO: See if " in argument is possible and handle if it is.
        if (argument.length() >= 2 && argument[0] == '-' && argument[1] == '-')
        {
            // Remove the two front dashes before parsing it as a long option name.
            argument.erase(0, 2);
            parseLongName(argument);
        }
        else if (argument.length() >= 1 && argument[0] == '-')
        {
            // Remove the front dash before parsing it as a short name option.
            argument.erase(0, 1);
            parseShortNameGroup(argument);
        }
        else
        {
            parseUnboundOption(argument);
        }
    }

    // Post parse validation checking. Verify required options are present, expected argument counts match, etc
    // before returning the results.
    results_->validate();
    return std::move(results_);
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::parseLongName(const std::string& longName)
{
    auto& option = findOptionByLongName(longName);

    // Mark the option set (for flags) and invoke any callbacks associated with parsing this option.
    // TODO: Handle if this option was specified multiple times.
    option.markSet(true);
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
void ArgParser::parseShortNameGroup(const std::string& argument)
{
    // Read each character in the argument as a potential option.
    for (size_t i = 0; i < argument.size(); ++i)
    {
        auto c = argument[i];

        // Look up the option named by the shortname, mark it as set and invoke any callback associated with the
        // option.
        auto& option = findOptionByShortName(c);

        option.markSet(true);
        option.invokeOnParsed();

        // TODO: Handle options with parameters.
        // TODO: Only allow one option with a paremeter and it has to be the last option in the group.
    }
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParser::parseUnboundOption(const std::string& argument)
{
    // Find the first unused positional argument and fill it.
    // TODO: Throw an exception.
    assert(nextPositionalOptionIndex_ < positionalLUT_.size());
    auto positionalOptionIndex = nextPositionalOptionIndex_++;
    
    auto& option = findOptionByLongName(positionalLUT_[positionalOptionIndex]);
    assert(option.desc().isPositional() && option.desc().positionalIndex() == positionalOptionIndex);

    // Set the positional option to have this argument as its parameter.
    option.addArgument(argument);

    // TODO: Handle positional options that accept more than one parameter.
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
    options_.insert({ name, desc });

    if (desc.hasShortName())
    {
        // TODO: Throw an exception.
        assert(shortNameToOptionLUT_.find(desc.shortName()) == shortNameToOptionLUT_.end() && "Duplicate short name");
        shortNameToOptionLUT_[desc.shortName()] = name;
    }

    if (desc.hasLongName())
    {
        // TODO: Throw an exception.
        assert(longNameToOptionLUT_.find(desc.longName()) == longNameToOptionLUT_.end() && "Duplicate long name");
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
bool ArgParser::tryFindOptionByLongName(
    const std::string& longName,
    const OptionState** optionState) const
{
    auto optNameItr = longNameToOptionLUT_.find(longName);

    if (optNameItr != longNameToOptionLUT_.end())
    {
        auto optionName = optNameItr->second;
        return results_->tryFindOption(optionName, optionState);
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
        return results_->tryFindOption(optionName, optionState);
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

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOptionByShortName(
    char shortName,
    const OptionState** optionState) const
{
    auto optNameItr = shortNameToOptionLUT_.find(shortName);

    if (optNameItr != shortNameToOptionLUT_.end())
    {
        auto optionName = optNameItr->second;
        return results_->tryFindOption(optionName, optionState);
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool ArgParser::tryFindOptionByShortName(
    char shortName,
    OptionState** optionState)
{
    auto optNameItr = shortNameToOptionLUT_.find(shortName);

    if (optNameItr != shortNameToOptionLUT_.end())
    {
        auto optionName = optNameItr->second;
        return results_->tryFindOption(optionName, optionState);
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
const OptionState& ArgParser::findOptionByShortName(char shortName) const
{
    const OptionState* optionState = nullptr;

    if (!tryFindOptionByShortName(shortName, &optionState))
    {
        throw UnknownShortNameException(shortName, __FILE__, __LINE__);
    }

    assert(optionState != nullptr);
    return *optionState;
}

//---------------------------------------------------------------------------------------------------------------------
OptionState& ArgParser::findOptionByShortName(char shortName)
{
    OptionState* optionState = nullptr;

    if (!tryFindOptionByShortName(shortName, &optionState))
    {
        throw UnknownShortNameException(shortName, __FILE__, __LINE__);
    }

    assert(optionState != nullptr);
    return *optionState;
}

//---------------------------------------------------------------------------------------------------------------------
std::string ArgParser::helpText() const
{
    std::stringstream ss;
    return "";
}

//---------------------------------------------------------------------------------------------------------------------
size_t ArgParser::generateNextUnusedPositionalIndex()
{
    return nextUnusedPositionalIndex_++;
}

//---------------------------------------------------------------------------------------------------------------------
std::vector<std::string> ArgParser::buildPositionalParameterLUT() const
{
    std::vector<std::string> positionals;

    // Go through all the registered options and collect all positional options into an unsorted array.
    // TODO: Verify only one positional option that accepts unlimited.
    for (const auto& kvp : options_)
    {
        const auto& option = kvp.second;

        if (option.isPositional())
        {
            positionals.push_back(option.name());
        }
    }

    // Sort positionals by their index making it so the returned array is keyed to the position index.
    std::sort(
        positionals.begin(),
        positionals.end(),
        [this](const std::string& oName1, const std::string & oName2) {
            const auto& o1 = findOptionByLongName(oName1);
            const auto& o2 = findOptionByLongName(oName2);

            return o1.desc().positionalIndex() < o2.desc().positionalIndex();
        });

    // Enforce there are no duplicate or missing indices before returning.
    for (size_t i = 0; i < positionals.size(); ++i)
    {
        const auto& option = findOptionByLongName(positionals[i]);

        // TODO: Throw exception.
        assert(option.desc().expectedArgumentCount() > 0 && "Positional optional expects at least one parameter");

        // TODO: Throw exception if failed.
        if (i == 0)
        {
            assert(option.desc().positionalIndex() == 0);
        }
        else
        {
            const auto& prevOption = findOptionByLongName(positionals[i - 1]);
            assert(option.desc().positionalIndex() == prevOption.desc().positionalIndex() + 1);
        }
    }

    return positionals;
}
