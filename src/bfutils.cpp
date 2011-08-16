/**
 * Brainfreeze Language Interpreter
 * (c) 2011 Scott MacDonald. All rights reserved.
 *
 * Utility functions
 */
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "bf.h"

const char CHR_EOF     = '\0';
const char CHR_PTR_INC = '>';
const char CHR_PTR_DEC = '<';
const char CHR_MEM_INC = '+';
const char CHR_MEM_DEC = '-';
const char CHR_WRITE   = '.';
const char CHR_READ    = ',';
const char CHR_JMP_FWD = '[';
const char CHR_JMP_BAC = ']';

bool BF::isInstruction( char c )
{
    return c == CHR_PTR_INC || 
           c == CHR_PTR_DEC || 
           c == CHR_MEM_INC || 
           c == CHR_MEM_DEC ||
           c == CHR_WRITE   || 
           c == CHR_READ    || 
           c == CHR_JMP_FWD || 
           c == CHR_JMP_BAC;
}

Instruction BF::convert( char c )
{
    switch( c )
    {
        case CHR_PTR_INC:
            return Instruction( OP_PTR_INC, 1 );
        case CHR_PTR_DEC:
            return Instruction( OP_PTR_DEC, 1 );
        case CHR_MEM_INC:
            return Instruction( OP_MEM_INC, 1 );
        case CHR_MEM_DEC:
            return Instruction( OP_MEM_DEC, 1 );
        case CHR_READ:
            return Instruction( OP_READ, 0 );
        case CHR_WRITE:
            return Instruction( OP_WRITE, 0 );
        case CHR_JMP_FWD:
            return Instruction( OP_JMP_FWD, 0 );
        case CHR_JMP_BAC:
            return Instruction( OP_JMP_BAC, 0 );
        default:
            return Instruction( OP_NOP, 0 );
    }
}

void BF::write( const BlockT& d )
{
    std::cout << static_cast<char>(d);
}

BlockT BF::read()
{
    BlockT d;

    if( std::cin >> d )
    {
        return d;
    }
    else    // if ( cin.eof() ) ... else ...
    {
        assert("Invalid input failure");
    }

    return d;
}

