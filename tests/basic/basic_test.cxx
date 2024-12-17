#include <catch2/catch_all.hpp>

TEST_CASE("Basic", "[example]")
{
    SECTION("Example 1")
    {
        REQUIRE(true);
    }
    SECTION("Example 2")
    {
        REQUIRE(true);
    }
}
