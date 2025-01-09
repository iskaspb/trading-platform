#include <catch2/catch_all.hpp>
#include <relay.h>

using namespace impl;

struct TestTraits
{
    enum
    {
        value = 42
    };
};

template <typename TraitsT> struct Node1
{
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
};

using EmptyAssembly = Assembly<TestTraits>;
struct Node1Assembly : Assembly<Node1Assembly, Node1>, TestTraits
{
};

struct Node12Assembly : Assembly<Node1Assembly, Node1, Node2>, TestTraits
{
};

TEST_CASE("Assembly", "[core]")
{
    SECTION("Empty assembly")
    {
        static_assert(EmptyAssembly::Traits::value == 42);
        static_assert(metal::size<EmptyAssembly::Nodes::List>::value == 0);
    }
    SECTION("Single node assembly without context")
    {
        static_assert(Node1Assembly::Traits::value == 42);
        static_assert(metal::size<Node1Assembly::Nodes::List>::value == 1);
    }
    SECTION("Assembly with context")
    {
        static_assert(Node1Assembly::Traits::value == 42);
        static_assert(metal::size<Node12Assembly::Nodes::List>::value == 2);
        static_assert(Node12Assembly::Nodes::Context::value == 123);
    }
}

TEST_CASE("Relay", "[core]")
{
    SECTION("Event echo")
    {
    }
}