// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include <cstdint>

namespace Brainfreeze
{
    /** Defines what operation an instruction should perform. */
    enum class OpcodeType : uint8_t
    {
        EndOfStream = 0,
        NoOperation = 1,
        PtrInc = 2,
        PtrDec = 3,
        MemInc = 4,
        MemDec = 5,
        Read = 6,
        Write = 7,
        JumpForward = 9,
        JumpBack = 10,
        FastJumpForward = 11,
        FastJumpBack = 12
    };

    /** Defines an executable Brainfreeze instruction. */
    struct instruction_t
    {
    public:
        using param_t = int16_t;

    public:
        /** Default constructor, defaults to a NOP instruction. */
        instruction_t() noexcept;

        /** Constructor that takes an opcode. */
        explicit instruction_t(OpcodeType op) noexcept;

        /** Constructor that takes an opcode and optional parameter value. */
        instruction_t(OpcodeType op, param_t value) noexcept;

        /** Get the opcode encoded in this instruction. */
        OpcodeType opcode() const noexcept;

        /** Set the opcode encoded in this instruction. */
        void setOpcode(OpcodeType op) noexcept;

        /** Check if this instruction has an opcode of the given type. */
        bool isA(OpcodeType op) const noexcept;

        /** Get the parameter value encoded in this instruction. */
        param_t param() const noexcept;

        /** Set the parameter value encoded in this instruction. */
        void setParam(param_t value) noexcept;

        /**
         * Increment parameter by the given amount.
         * An exception is thrown if the new value is too large or too small to be stored.
         */
        void incrementParam(param_t amount);

        /** Equality comparison operator. */
        bool operator ==(const instruction_t& other) const noexcept;

        /** Inequality comparison operator. */
        bool operator !=(const instruction_t& other) const noexcept;

    private:
        uint32_t data_ = 0;
    };
}
