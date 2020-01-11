#include "testhelpers.h"
#include <sstream>
#include <string>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

//=====================================================================================================================
MemoryMatcher::MemoryMatcher(size_t offset, BlockT expected)
    : offset_(offset), expected_(expected)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool MemoryMatcher::match(const BFProgram& program) const
{
    // TODO: Make sure offset is in range.
    auto actual = program.valueAt(offset_);

    if (expected_ == actual)
    {
        return true;
    }
    else
    {
        std::stringstream ss;

        ss << "Expected memory at offset " << std::to_string(offset_) << " to be "
            << std::to_string(expected_) << " but was "
            << std::to_string(actual);

        WARN(ss.str());

        return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string MemoryMatcher::describe() const
{
    std::stringstream ss;
    ss << "memory at offset " << std::to_string(offset_) << " is " << std::to_string(expected_);
    return ss.str();
}

//=====================================================================================================================
InstructionPointerMatcher::InstructionPointerMatcher(size_t expectedOffset)
    : expectedOffset_(expectedOffset)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool InstructionPointerMatcher::match(const BFProgram& program) const
{
    // TODO: Make sure offset is in range.
    auto actualOffset = program.instructionOffset();

    if (expectedOffset_ == actualOffset)
    {
        return true;
    }
    else
    {
        std::stringstream ss;

        ss << "Expected instruction pointer to be " << std::to_string(expectedOffset_) << " but was "
            << std::to_string(actualOffset);

        WARN(ss.str());

        return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string InstructionPointerMatcher::describe() const
{
    std::stringstream ss;
    ss << "instruction pointer is " << std::to_string(expectedOffset_);
    return ss.str();
}

//=====================================================================================================================
MemoryPointerMatcher::MemoryPointerMatcher(size_t expectedOffset)
    : expectedOffset_(expectedOffset)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool MemoryPointerMatcher::match(const BFProgram& program) const
{
    // TODO: Make sure offset is in range.
    auto actualOffset = program.memoryPointerOffset();

    if (expectedOffset_ == actualOffset)
    {
        return true;
    }
    else
    {
        std::stringstream ss;

        ss << "Expected memory pointer to be " << std::to_string(expectedOffset_) << " but was "
            << std::to_string(actualOffset);

        WARN(ss.str());

        return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string MemoryPointerMatcher::describe() const
{
    std::stringstream ss;
    ss << "memory pointer is " << std::to_string(expectedOffset_);
    return ss.str();
}

//=====================================================================================================================
MemoryMatcher Brainfreeze::TestHelpers::HasMemory(size_t offset, BlockT expected)
{
    return MemoryMatcher(offset, expected);
}

//---------------------------------------------------------------------------------------------------------------------
InstructionPointerMatcher Brainfreeze::TestHelpers::InstructionPointerIs(size_t expectedOffset)
{
    return InstructionPointerMatcher(expectedOffset);
}

//---------------------------------------------------------------------------------------------------------------------
MemoryPointerMatcher Brainfreeze::TestHelpers::MemoryPointerIs(size_t expectedOffset)
{
    return MemoryPointerMatcher(expectedOffset);
}
