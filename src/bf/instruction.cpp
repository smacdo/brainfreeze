// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

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
instruction_t::instruction_t(OpcodeType op, int16_t arg) noexcept
    : data_((0x000000FF & static_cast<uint8_t>(op)) | (arg << 8))
{
}

//---------------------------------------------------------------------------------------------------------------------
OpcodeType instruction_t::opcode() const noexcept
{
    return static_cast<OpcodeType>(data_ & 0x000000FF);
}

//---------------------------------------------------------------------------------------------------------------------
int16_t instruction_t::param() const noexcept
{
    return static_cast<int16_t>((data_ & 0xFFFFFF00) >> 8);
}

//---------------------------------------------------------------------------------------------------------------------
void instruction_t::setParam(int16_t p) noexcept
{
    data_ = (p << 8) | (0x000000FF & data_);
}

//---------------------------------------------------------------------------------------------------------------------
bool instruction_t::isA(OpcodeType op) const noexcept
{
    return opcode() == op;
}
