// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <cassert>
#include <stdexcept>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
Interpreter::Interpreter(std::vector<instruction_t> instructions)
    : instructions_(std::move(instructions)),
      writeFunction_(Brainfreeze::write),
      readFunction_(Brainfreeze::read)
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
    memory_.resize(cellCount_ * cellSize_);
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
        writeFunction_(*mp_);
        break;

    case OpcodeType::Read:
        *mp_ = readFunction_();
        break;

    case OpcodeType::JumpForward:
        // Only execute if byte at data pointer is zero
        if (*mp_ == 0)
        {
            // TODO: Handle case when param is zero (unoptimized, no jump stored).
            ip_ += ip_->param();
        }

        break;

    case OpcodeType::JumpBack:
        // Only execute if byte at data pointer is non-zero
        if (*mp_ != 0)
        {
            // TODO: Handle case when param is zero (unoptimized, no jump stored).
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

    ip_++;            // TODO: maybe integrate into switch
    return state_;
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::byte_t Interpreter::memoryAt(std::size_t offset) const
{
    assert(offset < memory_.size());
    return memory_[offset];
}

//---------------------------------------------------------------------------------------------------------------------
std::size_t Interpreter::instructionOffset() const
{
    return ip_ - instructions_.begin();
}

//---------------------------------------------------------------------------------------------------------------------
std::size_t Interpreter::memoryPointerOffset() const
{
    return mp_ - memory_.begin();
}

