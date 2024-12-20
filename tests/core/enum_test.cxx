#include <catch2/catch_all.hpp>
#include <util/enum.h>

ENUM(MyType, ZERO, ONE, TWO);
ENUM_CLASS(MyType2, ZERO, ONE, TWO);

TEST_CASE("Enum", "[core]")
{
    SECTION("Non-scoped enums")
    {
        REQUIRE(toString(ZERO) == "ZERO");
        REQUIRE(toString(ONE) == "ONE");
        REQUIRE(toString(TWO) == "TWO");
    }

    SECTION("Non-scoped enums with specified type")
    {
        REQUIRE(toString(MyType::ZERO) == "ZERO");
        REQUIRE(toString(MyType::ONE) == "ONE");
        REQUIRE(toString(MyType::TWO) == "TWO");
    }

    SECTION("Non-scoped enums wrong value")
    {
        REQUIRE(toString(static_cast<MyType>(-1)) == "MyType::<unknown>");
    }

    SECTION("Scoped enums")
    {
        using enum MyType2;
        REQUIRE(toString(ZERO) == "ZERO");
        REQUIRE(toString(ONE) == "ONE");
        REQUIRE(toString(TWO) == "TWO");
    }

    SECTION("Scoped enums with specified type")
    {
        REQUIRE(toString(MyType2::ZERO) == "ZERO");
        REQUIRE(toString(MyType2::ONE) == "ONE");
        REQUIRE(toString(MyType2::TWO) == "TWO");
    }

    SECTION("Scoped enums wrong value")
    {
        REQUIRE(toString(static_cast<MyType2>(-1)) == "MyType2::<unknown>");
    }
}
