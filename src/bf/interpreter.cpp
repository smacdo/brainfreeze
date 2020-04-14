// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <cassert>
#include <stdexcept>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
Interpreter::Interpreter(std::vector<instruction_t> instructions)
    : Interpreter(std::move(instructions), nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::Interpreter(
        std::vector<instruction_t> instructions,
        std::unique_ptr<Console> console)
    : instructions_(std::move(instructions)),
      console_(std::move(console))
{
}

//---------------------------------------------------------------------------------------------------------------------
void Interpreter::setCellCount(size_t count)
{
    if (state_ != RunState::NotStarted)
    {
        throw std::runtime_error("Cell count can only be set prior to execution");
    }

    cellCount_ = count;
}

//---------------------------------------------------------------------------------------------------------------------
void Interpreter::setCellSize(size_t bytes)
{
    if (bytes != 1 && bytes != 2 && bytes != 4 && bytes != 8)
    {
        throw std::runtime_error("Cell size must be a power of two");
    }

    if (state_ != RunState::NotStarted)
    {
        throw std::runtime_error("Cell size can only be set prior to execution");
    }

    cellSize_ = bytes;
}

//---------------------------------------------------------------------------------------------------------------------
void Interpreter::start()
{
    // TODO: If console required check that it is defined.
    assert(console_ != nullptr);
    memory_.resize(cellCount_ * cellSize_, 0);
    mp_ = memory_.begin();
    ip_ = instructions_.begin();

    state_ = RunState::Running;
}

//---------------------------------------------------------------------------------------------------------------------
void Interpreter::run()
{
    start();

    // Keep executing instructions until the end of the instruction stream is reached.
    while (state_ == RunState::Running)
    {
        runStep();
    }
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::RunState Interpreter::runStep()
{
    assert(state_ == RunState::Running);
    assert(ip_ < instructions_.end());

    switch (ip_->opcode())
    {
    case OpcodeType::PtrInc:
        assert(ip_->param() < memory_.end() - mp_);     // TODO: Test this boundary condition. MAYBE?
        mp_ += ip_->param();
        break;

    case OpcodeType::PtrDec:
        assert(ip_->param() <= mp_ - memory_.begin());
        mp_ -= ip_->param();
        break;
    
    case OpcodeType::MemInc:
        // TODO: Handle configurable memory blocks larger than 1 byte.
        // TODO: How should overflow be handled?
        *mp_ += static_cast<byte_t>(ip_->param());
        break;

    case OpcodeType::MemDec:
        // TODO: Handle configurable memory blocks larger than 1 byte.
        // TODO: How should overflow be handled?
        *mp_ -= static_cast<byte_t>(ip_->param());
        break;

    case OpcodeType::Write:
        console_->Write(*mp_);
        break;

    case OpcodeType::Read:
    {
        auto c = console_->Read();

        if (c == EOF)
        {
            switch (endOfStreamBehavior_)
            {
            case Interpreter::EndOfStreamBehavior::Zero:
                c = 0;
                break;

            case Interpreter::EndOfStreamBehavior::NegativeOne:
                c = (byte_t)-1;
                break;

            case Interpreter::EndOfStreamBehavior::NoChange:
                c = *mp_;

            default: // Use whatever was returned.
                break;
            }
        }
        
        *mp_ = c;
        break;
    }

    case OpcodeType::JumpForward:
        // Only execute if byte at data pointer is zero
        if (*mp_ == 0)
        {
            ip_ = Helpers::FindJumpTarget(instructions_.begin(), instructions_.end(), ip_);
        }

        break;

    case OpcodeType::JumpBack:
        // Only execute if byte at data pointer is non-zero
        if (*mp_ != 0)
        {
            ip_ = Helpers::FindJumpTarget(instructions_.begin(), instructions_.end(), ip_);
        }
        break;

    case OpcodeType::FastJumpForward:
        // Only execute if byte at data pointer is zero
        if (*mp_ == 0)
        {
            assert(ip_->param() > 0);
            ip_ += ip_->param();
        }

        break;

    case OpcodeType::FastJumpBack:
        // Only execute if byte at data pointer is non-zero
        if (*mp_ != 0)
        {
            assert(ip_->param() > 0);
            ip_ -= ip_->param();
        }
        break;

    case OpcodeType::EndOfStream:
        // Immediately return when end of stream is reached to prevent instruction pointer from being incremented or
        // other such nonsense.
        state_ = RunState::Finished;
        return RunState::Finished;

    default:
        throw std::runtime_error("unknown instruction opcode");
    }

    // Move to the next instruction and then return the running state of the interpreter.
    ip_++;
    return state_;
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::byte_t Interpreter::memoryAt(std::size_t offset) const
{
    assert(offset < memory_.size());
    return memory_[offset];
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::instruction_pointer_t Interpreter::instructionPointer() const
{
    // TODO: Remove
    // ip_ - instructions_.begin()
    return instruction_pointer_t(instructions_.begin(), ip_);
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::memory_pointer_t Interpreter::memoryPointer() const
{
    return memory_pointer_t(memory_.begin(), mp_);
}
