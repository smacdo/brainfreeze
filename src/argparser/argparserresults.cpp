// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include "exceptions.h"

#include <string>
#include <stdexcept>
#include <cassert>
#include <vector>

using namespace Brainfreeze::ArgParsing;

//---------------------------------------------------------------------------------------------------------------------
ArgParserResults::ArgParserResults(const std::unordered_map<std::string, OptionDesc>& options)
{
    for (const auto itr : options)
    {
        initOption(itr.second);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParserResults::initOption(const OptionDesc& option)
{
    assert(options_.find(option.name()) == options_.end());
    options_[option.name()] = std::make_unique<OptionState>(option);
}

//---------------------------------------------------------------------------------------------------------------------
const OptionState& ArgParserResults::option(const std::string& optionName) const
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
bool ArgParserResults::tryFindOption(
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
bool ArgParserResults::tryFindOption(
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
const std::string& ArgParserResults::programName() const
{
    return programName_;
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParserResults::setProgramName(const std::string& name)
{
    programName_ = name;
}

//---------------------------------------------------------------------------------------------------------------------
void ArgParserResults::validate() const
{
    for (const auto& kvp : options_)
    {
        const auto& option = kvp.second;

        // Check that required options were parsed.
        if (option->desc().isRequired() && !option->wasSet())
        {
            throw RequiredOptionMissingException(option->desc().name(), __FILE__, __LINE__);
        }

        // TODO: Make sure expected argument counts are correct.
    }
}
