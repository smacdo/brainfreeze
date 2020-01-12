// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"
#include <stdexcept>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
instruction_t::instruction_t() noexcept
    : instruction_t(OpcodeType::NoOperation)
{
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t::instruction_t(OpcodeType op) noexcept
    : instruction_t(op, 0)
{
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t::instruction_t(OpcodeType op, instruction_t::param_t value) noexcept
    : data_((0x000000FF & static_cast<uint8_t>(op)) | (value << 8))
{
}

//---------------------------------------------------------------------------------------------------------------------
OpcodeType instruction_t::opcode() const noexcept
{
    return static_cast<OpcodeType>(data_ & 0x000000FF);
}

//---------------------------------------------------------------------------------------------------------------------
instruction_t::param_t instruction_t::param() const noexcept
{
    return static_cast<instruction_t::param_t>((data_ & 0xFFFFFF00) >> 8);
}

//---------------------------------------------------------------------------------------------------------------------
void instruction_t::setParam(instruction_t::param_t value) noexcept
{
    data_ = (value << 8) | (0x000000FF & data_);
}

//---------------------------------------------------------------------------------------------------------------------
void instruction_t::incrementParam(instruction_t::param_t amount)
{
    auto current = param();

    if (std::numeric_limits<param_t>::max() - current < amount)
    {
        throw std::runtime_error("incremented instruction parameter value too large to be stored");
    }

    setParam(current + amount);
}

//---------------------------------------------------------------------------------------------------------------------
bool instruction_t::isA(OpcodeType op) const noexcept
{
    return opcode() == op;
}
