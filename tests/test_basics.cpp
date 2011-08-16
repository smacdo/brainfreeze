#include "bf.h"
#include <gtest.h>

namespace testing {
    AssertionResult CheckMem( BFProgram& app, size_t offset, BlockT expected )
    {
        BlockT actual = app.valueAt( offset );

        if ( actual == expected )
        {
            return AssertionSuccess();
        }
        else
        {
            return AssertionFailure()
                << "Value at memory address " << offset
                << "was: "   << actual        << ". "
                << "Expected: " << expected;
        }
    }

    AssertionResult CheckIPtr( BFProgram& app, size_t expected )
    {
        size_t actual = app.instructionOffset();

        if ( expected == actual )
        {
            return AssertionSuccess();
        }
        else
        {
            return AssertionFailure()
                << "Instruction pointer was: " << actual
                << ". Expected: "              << expected;
        }
    }

    AssertionResult CheckMPtr( BFProgram& app, size_t expected )
    {
        size_t actual = app.memoryPointerOffset();

        if ( expected == actual )
        {
            return AssertionSuccess();
        }
        else
        {
            return AssertionFailure()
                << "Memory pointer was: " << actual
                << ". Expected: "         << expected;
        }
    }
}

using namespace testing;

TEST(BrainfreezeLanguage, InstructionClass)
{
    Instruction instr( 3, 42 );

    ASSERT_EQ( static_cast<uint8_t>(3),  instr.opcode() );
    ASSERT_EQ( static_cast<uint32_t>(42), instr.param() );
    ASSERT_TRUE( instr.isA(3) );

    instr.setParam( 96 );

    ASSERT_EQ( static_cast<uint8_t>(3), instr.opcode() );
    ASSERT_EQ( static_cast<uint32_t>(96), instr.param() );
    ASSERT_TRUE( instr.isA(3) );
}

TEST(BrainfreezeLanguage, Empty)
{
    BFProgram app( std::string( "" ) );
    app.run();

    CheckMem(app,0,0);
    CheckIPtr(app,0);
    CheckMPtr(app,0);
}

TEST(BrainfreezeLanguage, AddOp)
{
    BFProgram app( std::string("+") );
    app.run();

    CheckMem(app,0,1);
}

TEST(BrainfreezeLanguage, SubOp)
{
    BFProgram app( std::string("-") );
    app.run();

    CheckMem(app,0,-1);
}

TEST(BrainfreezeLanguage, LeftOp)
{
    BFProgram app( std::string(">") );
    app.run();

    CheckMem(app,0,0);
    CheckMPtr(app,1);
}

TEST(BrainfreezeLanguage, RightOp)
{
    BFProgram app( std::string(">><") );
    app.run();

    CheckMPtr(app,1);
}

TEST(BrainfreezeLanguage, Clear)
{
    BFProgram app( std::string("+++++[-]") );
    app.run();

    CheckMem(app,0,0);
}

TEST(BrainfreezeLanguage, ClearAllCells)
{
    BFProgram app( std::string(">+++++>++++>+++>++>+[[-]<]") );
    app.run();

    CheckMem(app,0,0);
    CheckMem(app,1,0);
    CheckMem(app,2,0);
    CheckMem(app,3,0);
    CheckMem(app,4,0);
}

TEST(BrainfreezeLanguage, Rewind)
{
    BFProgram app( std::string(">>+>+[<]>") );
    app.run();

    CheckMPtr(app,2);
}

TEST(BrainfreezeLanguage, FastForward)
{
    // 11101
    BFProgram app( std::string("+>+>+>>+<<<<[>]<") );
    app.run();

    CheckMPtr(app,2);
}

TEST(BrainfreezeLanguage, DestructiveAdd)
{
    // 3 + 2, 5
    BFProgram app( std::string("+++>++<[->+<]") );
    app.run();

    CheckMem(app,0,0);
    CheckMem(app,1,5);
}

TEST(BrainfreezeLanguage, NonDestructiveAdd)
{
    // 3 + 2, 5
    BFProgram app( std::string("+++>++<[->+>+<<]>>[-<<+>>]") );
    app.run();

    CheckMem(app,0,3);
    CheckMem(app,1,5);
}

TEST(BrainfreezeLanguage, NonDestructiveCopy)
{
    // 4 <-> 2
    BFProgram app( std::string("++++>++<>[-]>[-]<<[->+>+<<]>>[-<<+>>]<<") );
    app.run();

    CheckMem(app,0,4);
    CheckMem(app,1,4);
}
