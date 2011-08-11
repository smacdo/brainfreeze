#include "bf.h"
#include <cassert>
#include <iostream>
#include <vector>

class UnitTest
{
public:
    UnitTest()
        : m_line( 0 ),
          m_reason( "N/A" ),
          m_failed( false )
    {
    }

    virtual void run() = 0;
    virtual const char* name() const = 0;
    
    int line() const { return m_line; }
    bool failed() const { return m_failed; }
    const char * reason() const { return m_reason; }

protected:
    int m_line;
    const char * m_reason;
    bool m_failed;
};

std::vector<UnitTest*> GTests;

class UnitTestContainer
{
public:
    
    UnitTestContainer( UnitTest* test )
    {
        GTests.push_back( test );
    }
};

#define TEST(X) class UnitTest##X : public UnitTest \
{ public: void run(); const char* name() const; }; \
UnitTestContainer container##X( new UnitTest##X() ); \
const char* UnitTest##X::name() const { return #X; } \
void UnitTest##X::run()

#define assertEquals(X,Y) if((X)!=(Y)){ m_failed = true; m_line = __LINE__; m_reason = "Equality assertion failed"; return; }
#define assertNotEquals(X,Y) if ((X)==(Y)) { m_failed = true; m_line = __LINE__; m_reason "Inequality assertion failed"; return; }
#define assertTrue(X) if((X)==false){ m_failed = true; m_line = __LINE__; m_reason = "Assert true failed"; return; }
#define assertFalse(X) if ((X)==true){ m_failed = true; m_line = __LINE__; m_reason = "Assert false failed"; return; }

#define CODE(X) BFProgram app(std::string(X)); app.run();
#define CHECKMEM(X,Y) if(app.valueAt((X))!=(Y)){ m_failed = true; m_line = __LINE__; m_reason = "Memory value assertion failed"; return; }
#define CHECKIPTR(P) if(app.instructionOffset()!=(P)){ m_failed = true; m_line = __LINE__; m_reason = "Instruction position assertion failed"; return; }
#define CHECKMPTR(P) if(app.memoryPointerOffset()!=(P)){ m_failed = true; m_line = __LINE__; m_reason = "Memory position assertion failed"; return; }

TEST(InstructionClass)
{
    //
    // Instructions test
    //
    Instruction instr( 3, 42 );

    assertEquals( instr.opcode(), 3 );
    assertEquals( instr.param(), 42 );
    assertTrue(   instr.isA(3) );

    instr.setParam( 96 );

    assertEquals( instr.opcode(), 3 );
    assertEquals( instr.param(), 96 );
    assertTrue( instr.isA(3) );
}

TEST(Empty)
{
    CODE("");
    CHECKMEM(0,0);
    CHECKIPTR(0);
    CHECKMPTR(0);
}

TEST(AddOp)
{
    CODE("+");
    CHECKMEM(0,1);
}

TEST(SubOp)
{
    CODE("-");
    CHECKMEM(0,-1);
}

TEST(LeftOp)
{
    CODE(">");
    CHECKMEM(0,0);
    CHECKMPTR(1);
}

TEST(RightOp)
{
    CODE(">><");
    CHECKMPTR(1);
}

TEST(Clear)
{
    CODE("+++++[-]");
    CHECKMEM(0,0);
}

TEST(ClearAllCells)
{
    CODE(">+++++>++++>+++>++>+[[-]<]");
    CHECKMEM(0,0);
    CHECKMEM(1,0);
    CHECKMEM(2,0);
    CHECKMEM(3,0);
    CHECKMEM(4,0);
}

TEST(Rewind)
{
    CODE(">>+>+[<]>");
    CHECKMPTR(2);
}

TEST(FastForward)
{
    // 11101
    CODE("+>+>+>>+<<<<[>]<");
    CHECKMPTR(2);
}

TEST(DestructiveAdd)
{
    // 3 + 2, 5
    CODE("+++>++<[->+<]");
    CHECKMEM(0,0);
    CHECKMEM(1,5);
}

TEST(NonDestructiveAdd)
{
    // 3 + 2, 5
    CODE("+++>++<[->+>+<<]>>[-<<+>>]");
    CHECKMEM(0,3);
    CHECKMEM(1,5);
}

TEST(NonDestructiveCopy)
{
    // 4 <-> 2
    CODE("++++>++<>[-]>[-]<<[->+>+<<]>>[-<<+>>]<<");
    CHECKMEM(0,4);
    CHECKMEM(1,4);
}

void runTests()
{
    int passed = 0;
    int failed = 0;
    int number = 0;

    std::cout << std::endl;
    std::cout << "---------- Executing Tests ---------- " << std::endl;
    for ( int i = 0; i < GTests.size(); ++i )
    {
        // Run the test
        GTests[i]->run();

        if ( GTests[i]->failed() == false )
        {
            std::cout << "PASSED: " << GTests[i]->name() << std::endl;
            passed++;
        }
        else
        {
            std::cout << "FAILED: " << GTests[i]->name() << std::endl;
            std::cout << " Line - " << GTests[i]->line() << std::endl;
            std::cout << " Msg  - " << GTests[i]->reason() << std::endl;
            failed++;
        }

        // Clean up
        delete GTests[i];
    }

    //
    // Summary
    //
    std::cout << std::endl;
    std::cout << "Ran " << (passed+failed) << " tests with "
              << failed << " failures and "
              << passed << " passes." << std::endl;

    if ( failed == 0 )
    {
        std::cout << "All tests passed!" << std::endl;
    }
    else
    {
        std::cout << "There were test failures :(" << std::endl;
    }
}
