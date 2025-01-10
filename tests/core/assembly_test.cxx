#include <assembly.h>
#include <catch2/catch_all.hpp>

using namespace core;

struct TestTraits
{
    enum
    {
        value = 42
    };
};

template <typename TraitsT> struct Node1
{
    Node1(const std::string &name = "Node1") : name_(name)
    {
    }
    const std::string &name() const
    {
        return name_;
    }

  private:
    std::string name_;
};
template <typename TraitsT> struct Node2
{
    struct ContextMixin
    {
        enum
        {
            value = 123
        };
    };

    const std::string &name() const
    {
        return name_;
    }

  private:
    std::string name_{"Node2"};
};

using EmptyAssembly = Assembly<TestTraits>;
struct Assembly1 : Assembly<Assembly1, Node1>, TestTraits
{
};

struct Assembly12 : Assembly<Assembly1, Node1, Node2>, TestTraits
{
};

TEST_CASE("Assembly", "[core]")
{
    SECTION("Empty assembly")
    {
        static_assert(EmptyAssembly::Traits::value == 42);
        static_assert(metal::size<EmptyAssembly::Nodes>::value == 0);
    }
    SECTION("Single node assembly without context")
    {
        static_assert(Assembly1::Traits::value == 42);
        static_assert(metal::size<Assembly1::Nodes>::value == 1);
    }
    SECTION("Assembly with context")
    {
        static_assert(Assembly1::Traits::value == 42);
        static_assert(metal::size<Assembly12::Nodes>::value == 2);
        static_assert(Assembly12::Holder::Context::value == 123);
    }
    SECTION("Construct assembly without parameter")
    {
        Assembly12::Holder holder;
        REQUIRE(holder.get<0>().name() == "Node1");
        REQUIRE(holder.get<1>().name() == "Node2");
    }
    SECTION("Construct assembly with parameter")
    {
        Assembly12::Holder holder("OtherNameForNode1");
        REQUIRE(holder.get<0>().name() == "OtherNameForNode1");
        REQUIRE(holder.get<1>().name() == "Node2");
    }
}
