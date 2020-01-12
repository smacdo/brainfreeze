// Copyright 2009-2020, Scott MacDonald.
#include "bf.h"

#include <iostream>

using namespace Brainfreeze;

//---------------------------------------------------------------------------------------------------------------------
void Brainfreeze::Details::Write(Interpreter::byte_t d)
{
    std::cout << static_cast<char>(d);
}

//---------------------------------------------------------------------------------------------------------------------
Interpreter::byte_t Brainfreeze::Details::Read()
{
    Interpreter::byte_t d;
    std::cin >> d;

    return d;
}
