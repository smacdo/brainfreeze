#include "testhelpers.h"
#include <sstream>
#include <string>

using namespace Brainfreeze;
using namespace Brainfreeze::TestHelpers;

//=====================================================================================================================
MemoryMatcher::MemoryMatcher(size_t address, Interpreter::byte_t expected)
    : address_(address), expected_(expected)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool MemoryMatcher::match(const Interpreter& program) const
{
    // TODO: Make sure offset is in range.
    auto actual = program.memoryAt(address_);

    if (expected_ == actual)
    {
        return true;
    }
    else
    {
        std::stringstream ss;

        ss << "Expected memory at address " << std::to_string(address_) << " to be "
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
    ss << "memory at address " << std::to_string(address_) << " is " << std::to_string(expected_);
    return ss.str();
}

//=====================================================================================================================
InstructionPointerMatcher::InstructionPointerMatcher(std::size_t expectedAddress)
    : expectedAddress_(expectedAddress)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool InstructionPointerMatcher::match(const Interpreter::instruction_pointer_t& actualIP) const
{
    if (expectedAddress_ == actualIP.address())
    {
        return true;
    }
    else
    {
        std::stringstream ss;
        ss << "Expected instruction pointer address to be " << expectedAddress_ << " but was " << actualIP;

        WARN(ss.str());

        return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string InstructionPointerMatcher::describe() const
{
    std::stringstream ss;
    ss << "instruction pointer address is " << expectedAddress_;
    return ss.str();
}

//=====================================================================================================================
MemoryPointerMatcher::MemoryPointerMatcher(std::size_t expectedAddress)
    : expectedAddress_(expectedAddress)
{
}

//---------------------------------------------------------------------------------------------------------------------
bool MemoryPointerMatcher::match(const Interpreter::memory_pointer_t& actualMP) const
{
    if (expectedAddress_ == actualMP.address())
    {
        return true;
    }
    else
    {
        std::stringstream ss;
        ss << "Expected memory pointer address to be " << expectedAddress_ << " but was " << actualMP;

        WARN(ss.str());

        return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------
std::string MemoryPointerMatcher::describe() const
{
    std::stringstream ss;
    ss << "memory pointer address is " << expectedAddress_;
    return ss.str();
}

//=====================================================================================================================
MemoryMatcher Brainfreeze::TestHelpers::HasMemory(size_t offset, Interpreter::byte_t expected)
{
    return MemoryMatcher(offset, expected);
}

//---------------------------------------------------------------------------------------------------------------------
InstructionPointerMatcher Brainfreeze::TestHelpers::InstructionPointerIs(std::size_t expectedAddress)
{
    return InstructionPointerMatcher(expectedAddress);
}

//---------------------------------------------------------------------------------------------------------------------
MemoryPointerMatcher Brainfreeze::TestHelpers::MemoryPointerIs(std::size_t expectedAddress)
{
    return MemoryPointerMatcher(expectedAddress);
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::TestHelpers::operator <<(std::ostream& os, Interpreter::instruction_pointer_t ip)
{
    os << std::to_string(ip.address());
    return os;
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::TestHelpers::operator <<(std::ostream& os, Interpreter::memory_pointer_t mp)
{
    os << std::to_string(mp.address());
    return os;
}
