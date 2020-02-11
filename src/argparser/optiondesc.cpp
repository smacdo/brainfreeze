// Copyright 2009-2020, Scott MacDonald.
#include "argparser.h"

#include <string>
#include <stdexcept>
#include <cassert>
#include <limits>

using namespace Brainfreeze::ArgParsing;

//---------------------------------------------------------------------------------------------------------------------
bool OptionDesc::isPositional() const
{
    return positionalIndex_.has_value();
}

//---------------------------------------------------------------------------------------------------------------------
size_t OptionDesc::positionalIndex() const
{
    assert(positionalIndex_.has_value());
    return positionalIndex_.value();
}

//---------------------------------------------------------------------------------------------------------------------
void OptionDesc::setPositionalIndex(size_t positionalIndex)
{
    assert(expectedArgumentCount_ > 0 && "Positional options must take at least one parameter");
    positionalIndex_ = positionalIndex;
}

//---------------------------------------------------------------------------------------------------------------------
size_t OptionDesc::expectedArgumentCount() const
{
    if (expectedArgumentCount_.has_value())
    {
        return expectedArgumentCount_.value();
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
void OptionDesc::setExpectedArgumentCount(size_t count)
{
    expectedArgumentCount_ = count;
}

//---------------------------------------------------------------------------------------------------------------------
bool OptionDesc::expectsArguments() const
{
    if (expectedArgumentCount_.has_value())
    {
        return expectedArgumentCount_.value() > 0;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------
bool OptionDesc::didSetExpectedArgumentCount() const
{
    return expectedArgumentCount_.has_value();
}
