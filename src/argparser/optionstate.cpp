// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"
#include <cassert>

using namespace Brainfreeze::ArgParsing;

//---------------------------------------------------------------------------------------------------------------------
OptionState::OptionState(OptionDesc desc)
    : desc_(desc)
{
}

//---------------------------------------------------------------------------------------------------------------------
void OptionState::invokeOnParsed()
{
    if (desc_.hasOnParsed())
    {
        desc_.onParsed()();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void OptionState::invokeOnArgument(const std::string& value)
{
    if (desc_.hasOnArgument())
    {
        desc_.onArgument()(value);
    }
}

//---------------------------------------------------------------------------------------------------------------------
size_t OptionState::argumentCount() const
{
    return arguments_.size();
}

//---------------------------------------------------------------------------------------------------------------------
bool OptionState::expectsMoreArguments() const
{
    return arguments_.size() < desc_.expectedArgumentCount();
}

//---------------------------------------------------------------------------------------------------------------------
size_t OptionState::expectedArgumentsRemaining() const
{
    assert(expectsMoreArguments());
    return desc_.expectedArgumentCount() - arguments_.size();
}

//---------------------------------------------------------------------------------------------------------------------
void OptionState::addArgument(const std::string& argument)
{
    // TODO: Validate that arguments expected and this would not add too many arguments.
    assert(arguments_.size() + 1 <= desc_.expectedArgumentCount());
    
    arguments_.push_back(argument);
    markSet(true);

    invokeOnArgument(argument);
}

//---------------------------------------------------------------------------------------------------------------------
const std::vector<std::string>& OptionState::arguments() const
{
    return arguments_;
}

//---------------------------------------------------------------------------------------------------------------------
const std::string& OptionState::argumentValue() const
{
    assert(argumentCount() > 0);
    assert(arguments_.size() > 0);

    return arguments_[0];
}
