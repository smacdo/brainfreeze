// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"

#include <string>
#include <stdexcept>
#include <cassert>
#include <limits>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder::ParameterBuilder(ArgParser::parameter_t* paramToBuild)
    : param_(paramToBuild)
{
    assert(paramToBuild != nullptr);
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::shortName(char name)
{
    param_->shortName = name;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::longName(const std::string& name)
{
    // TODO: If this changes need to notify parent.
    param_->longName = name;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::description(const std::string& description)
{
    param_->description = description;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::expectedArgumentCount(size_t count)
{
    param_->expectedArgumentCount = count;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::onParam(ArgParser::parameter_t::parameter_callback_t&& callback)
{
    param_->onParam = callback;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::onArgument(ArgParser::parameter_t::argument_callback_t&& callback)
{
    param_->onArgument = callback;
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::isFlag(bool isFlag)
{
    // TODO: Handle this better.
    if (isFlag)
    {
        param_->expectedArgumentCount = 0;
    }

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::bindString(std::string* binding)
{
    // TODO: Throw an exception if there is already a onArgument parameter set.
    // TODO: Throw an exception if expected parameter count set to > 1.
    // TODO: Once bound make sure other values like expectedParameterCount / other bindings are not set.
    // TODO: Do not have more than one binding.
    assert(binding != nullptr);
    assert(param_->expectedArgumentCount == 1);   // these should be runtime exceptions not assers
    assert(param_->onArgument == nullptr);        // these should also be less flimsy

    if (binding != nullptr)
    {
        // TODO: can we make this more efficient? will this lead to a heap alloc for each param this is used on?
        onArgument([binding](const std::string& argument) { *binding = argument; });
    }

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::bindInt(int* binding)
{
    // TODO: Throw an exception if there is already a onArgument parameter set.
    // TODO: Throw an exception if expected parameter count set to > 1.
    // TODO: Once bound make sure other values like expectedParameterCount / other bindings are not set.
    // TODO: Do not have more than one binding.
    assert(binding != nullptr);
    assert(param_->expectedArgumentCount == 1);   // these should be runtime exceptions not assers
    assert(param_->onArgument == nullptr);        // these should also be less flimsy

    if (binding != nullptr)
    {
        // TODO: can we make this more efficient? will this lead to a heap alloc for each param this is used on?
        onArgument([binding](const std::string& argument) {
            // TODO: Perform argument validation (and throw an exception) here before parsing the int.
            *binding = std::stoi(argument);
        });
    }

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
ParameterBuilder& ParameterBuilder::bindSize(size_t* binding)
{
    // TODO: Throw an exception if there is already a onArgument parameter set.
    // TODO: Throw an exception if expected parameter count set to > 1.
    // TODO: Once bound make sure other values like expectedParameterCount / other bindings are not set.
    // TODO: Do not have more than one binding.
    assert(binding != nullptr);
    assert(param_->expectedArgumentCount == 1);   // these should be runtime exceptions not assers
    assert(param_->onArgument == nullptr);        // these should also be less flimsy

    if (binding != nullptr)
    {
        // TODO: can we make this more efficient? will this lead to a heap alloc for each param this is used on?
        onArgument([binding](const std::string& argument) {
            // TODO: Perform argument validation (and throw an exception) here before parsing the int.
            long long size = stoll(argument);

            if (size >= 0 && static_cast<unsigned long long>(size) <= std::numeric_limits<size_t>::max())
            {
                *binding = std::stoi(argument);
            }
            else
            {
                // TODO: Throw a better exception.
                throw std::runtime_error("size out of range, put a better exception in here");
            }});
    }

    return *this;
}
