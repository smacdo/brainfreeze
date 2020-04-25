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
std::vector<instruction_t> Brainfreeze::TestHelpers::Compile(
    const std::string& code,
    std::function<void(Compiler&)>&& configureCallback)
{
    Compiler compiler;
    
    if (configureCallback != nullptr)
    {
        configureCallback(compiler);
    }

    return compiler.compile(code);
}

//---------------------------------------------------------------------------------------------------------------------
std::vector<instruction_t> Brainfreeze::TestHelpers::Compile(const std::string& code)
{
    return Compile(code, nullptr);
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter Brainfreeze::TestHelpers::CreateInterpreter(const std::string& code)
{
    return CreateInterpreter(
        code,
        []() { return Interpreter::byte_t{}; },
        [](Interpreter::byte_t) {});
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter Brainfreeze::TestHelpers::CreateInterpreter(
    const std::string& code,
    std::function<Interpreter::byte_t(void)>&& readFunc,
    std::function<void(Interpreter::byte_t)>&& writeFunc)
{
    Compiler compiler;
    return Interpreter(
        compiler.compile(code),
        std::make_unique<TestableConsole>(std::move(readFunc), std::move(writeFunc)));
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::operator <<(std::ostream& os, Interpreter::instruction_pointer_t ip)
{
    os << std::to_string(ip.address());
    return os;
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::operator <<(std::ostream& os, Interpreter::memory_pointer_t mp)
{
    os << std::to_string(mp.address());
    return os;
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::operator <<(std::ostream& os, instruction_t instruction)
{
    os << Brainfreeze::Helpers::AsName(instruction.opcode());

    if (instruction.param() != 0)
    {
        os << ":" << instruction.param();
    }

    return os;
}

//---------------------------------------------------------------------------------------------------------------------
std::ostream& Brainfreeze::operator <<(std::ostream& os, std::vector<instruction_t>::const_iterator itr)
{
    os << *itr;
    return os;
}

//=====================================================================================================================
TestableConsole::TestableConsole(
        std::function<char(void)>&& readFunc,
        std::function<void(char)>&& writeFunc)
    : readFunction_(std::move(readFunc)),
      writeFunction_(std::move(writeFunc))
{
}

//---------------------------------------------------------------------------------------------------------------------
void TestableConsole::write(char d)
{
    writeFunction_(d);
}

//---------------------------------------------------------------------------------------------------------------------
char TestableConsole::read()
{
    return readFunction_();
}

//---------------------------------------------------------------------------------------------------------------------
std::function<char(void)> TestableConsole::readFunction() const
{
    return readFunction_;
}

//---------------------------------------------------------------------------------------------------------------------
void TestableConsole::setReadFunction(std::function<char(void)> func)
{
    readFunction_ = std::move(func);
}

//---------------------------------------------------------------------------------------------------------------------
std::function<void(char)> TestableConsole::writeFunction() const
{
    return writeFunction_;
}

//---------------------------------------------------------------------------------------------------------------------
void TestableConsole::setWriteFunction(std::function<void(char)> func)
{
    writeFunction_ = std::move(func);
}
