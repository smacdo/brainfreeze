#include "bf.h"
#include "testhelpers.h"
#include <catch2/catch.hpp>

using namespace Brainfreeze;
using namespace Brainfreeze::Helpers;
using namespace Brainfreeze::TestHelpers;

TEST_CASE("find the matching back jump from a forward jump", "[jumpsearch]")
{
    SECTION("when there is only one matching target")
    {
        auto il = Compile("[]");
        REQUIRE(il.cbegin() + 1 == FindJumpTarget(il.begin(), il.end(), il.begin()));
    }

    SECTION("when there is only one matching target and stuff inbetween")
    {
        auto il = Compile("-[+.]");
        REQUIRE(il.cbegin() + 4 == FindJumpTarget(il.begin(), il.end(), il.begin() + 1));
    }

    SECTION("when there are nested jumps")
    {
        //                 012345678
        auto il = Compile("[[.][]][]");
        REQUIRE(il.cbegin() + 6 == FindJumpTarget(il.begin(), il.end(), il.begin() + 0));
        REQUIRE(il.cbegin() + 3 == FindJumpTarget(il.begin(), il.end(), il.begin() + 1));
        REQUIRE(il.cbegin() + 5 == FindJumpTarget(il.begin(), il.end(), il.begin() + 4));
        REQUIRE(il.cbegin() + 8 == FindJumpTarget(il.begin(), il.end(), il.begin() + 7));
    }
}

TEST_CASE("find the matching forward jump from a back jump", "[jumpsearch]")
{
    SECTION("when there is only one matching target")
    {
        auto il = Compile("[]");
        REQUIRE(il.cbegin() + 0 == FindJumpTarget(il.begin(), il.end(), il.begin() + 1));
    }

    SECTION("when there is only one matching target and stuff inbetween")
    {
        auto il = Compile("-[+.]");
        REQUIRE(il.cbegin() + 1 == FindJumpTarget(il.begin(), il.end(), il.begin() + 4));
    }

    SECTION("when there are nested jumps")
    {
        //                 012345678
        auto il = Compile("[[.][]][]");
        REQUIRE(il.cbegin() + 0 == FindJumpTarget(il.begin(), il.end(), il.begin() + 6));
        REQUIRE(il.cbegin() + 1 == FindJumpTarget(il.begin(), il.end(), il.begin() + 3));
        REQUIRE(il.cbegin() + 4 == FindJumpTarget(il.begin(), il.end(), il.begin() + 5));
        REQUIRE(il.cbegin() + 7 == FindJumpTarget(il.begin(), il.end(), il.begin() + 8));
    }
}
