// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"

#include <string>
#include <stdexcept>
#include <cassert>
#include <limits>

using namespace Brainfreeze::ArgParsing;

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder::OptionBuilder(std::string name, std::string longNameIn, ArgParser& parent)
    : parent_(parent),
      desc_(name),
      name_(name)
{
    assert(name_.size() > 0);
    longName(longNameIn);
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder::~OptionBuilder()
{
    // TODO: Perform configuration validation before committing.
    // Add the option to the arg parser.
    parent_.registerOption(name_, desc_);
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::shortName(char shortName)
{
    desc_.setShortName(shortName);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::longName(const std::string& longName)
{
    assert(!longName.empty());
    desc_.setLongName(longName);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::description(const std::string& description)
{
    desc_.setDescription(description);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::expectsArgument()
{
    return expectsArguments(1);
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::expectsArguments(size_t count)
{
    desc_.setExpectedArgumentCount(count);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::positional()
{
    positional(parent_.generateNextUnusedPositionalIndex());
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::positional(size_t expectedIndex)
{
    assert(desc_.expectedArgumentCount() > 0 && "Positional argument must take parameters");

    desc_.setPositionalIndex(expectedIndex);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::onParsed(parsed_callback_t&& callback)
{
    assert(!desc_.hasOnParsed() && "Multiple onParsed callbacks not supported");
    desc_.setOnParsed(std::move(callback));
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::onArgument(argument_callback_t&& callback)
{
    assert(!desc_.hasOnArgument() && "Multiple onArgument callbacks not supported");
    desc_.setOnArgument(std::move(callback));
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::expectsString(std::string* binding)
{
    // Debug checks to ensure the option is being built in a sane fashion.
    //  (Don't throw in runtime because these errors are build time problems and not user input errors).
    assert(binding != nullptr && "Must provide a valid binding target");
    assert(desc_.onArgument() == nullptr && "Multiple argument callbacks not supported");
    assert(!desc_.didSetExpectedArgumentCount() || desc_.expectedArgumentCount() == 1);

    if (binding != nullptr)
    {
        desc_.setExpectedArgumentCount(1);
        onArgument([binding](const std::string& argument) { *binding = argument; });
    }

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::expectsInt(int* binding)
{
    // Debug checks to ensure the option is being built in a sane fashion.
    //  (Don't throw in runtime because these errors are build time problems and not user input errors).
    assert(binding != nullptr && "Must provide a valid binding target");
    assert(desc_.onArgument() == nullptr && "Multiple argument callbacks not supported");
    assert(!desc_.didSetExpectedArgumentCount() || desc_.expectedArgumentCount() == 1);

    if (binding != nullptr)
    {
        desc_.setExpectedArgumentCount(1);

        onArgument([binding](const std::string& argument) {
            // TODO: Perform argument validation (and throw an exception) here before parsing the int.
            *binding = std::stoi(argument);
        });
    }

    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
OptionBuilder& OptionBuilder::expectsSize(size_t* binding)
{
    // Debug checks to ensure the option is being built in a sane fashion.
    //  (Don't throw in runtime because these errors are build time problems and not user input errors).
    assert(binding != nullptr && "Must provide a valid binding target");
    assert(desc_.onArgument() == nullptr && "Multiple argument callbacks not supported");
    assert(!desc_.didSetExpectedArgumentCount() || desc_.expectedArgumentCount() == 1);

    if (binding != nullptr)
    {
        desc_.setExpectedArgumentCount(1);

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
