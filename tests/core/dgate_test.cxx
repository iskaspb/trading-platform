#include <catch2/catch_all.hpp>
#include <dgate.h>

struct Functor
{
    int operator()(int i)
    {
        this->i = i;
        return i;
    }
    std::string operator()(std::string s)
    {
        this->s = s;
        return s;
    }

    int i = 0;
    std::string s;
};

TEST_CASE("DGate", "[core]")
{
    SECTION("Lambda DGate : a case when delegate 'contains' instances of lambdas")
    {
        //...exact type of DGate is DGate<lambda1, lambda2> (which is implementation specific)
        DGate dgate([](int i) { return i; }, [](std::string s) { return s; });
        REQUIRE(dgate(1) == 1);
        REQUIRE(dgate("2") == "2");
    }

    SECTION("Self-DGate : a case when delegate inherits from proxied class")
    {
        /*
        This is a rare case when delegate inherits from a proxied class,
        because it kills the purpose of a delegate (to represent a proxied object but not to be the one)
        */
        DGate dgate(Functor{});
        static_assert(std::is_same_v<decltype(dgate), DGate<Functor>>);

        REQUIRE(dgate(1) == 1);
        REQUIRE(dgate.i == 1);

        REQUIRE(dgate("2") == "2");
        REQUIRE(dgate.s == "2");
    }

    SECTION("Explicit Self-DGate type definition")
    {
        //...the same as previous case, but with explicit type definition - just to show how that it's possible
        DGate<Functor> dgate;

        REQUIRE(dgate(1) == 1);
        REQUIRE(dgate.i == 1);

        REQUIRE(dgate("2") == "2");
        REQUIRE(dgate.s == "2");
    }

    SECTION("Reference DGate : a case when delegate references a proxied class")
    {
        /*
        This is most typical case, however normally we want to define the type of delegate in advance.
        It's better presented in EventsDGate test case (see below)
        */
        Functor test;
        DGate dgate(std::ref(test));
        static_assert(std::is_same_v<decltype(dgate), DGate<std::reference_wrapper<Functor>>>);

        REQUIRE(dgate(1) == 1);
        REQUIRE(test.i == 1);

        REQUIRE(dgate("2") == "2");
        REQUIRE(test.s == "2");
    }
}

TEST_CASE("DGateRef", "[core]")
{
    /*
    DGateRef is a "lightweight" version of DGate, which is used to reference a proxied object.
    Compare to DGate, DGateRef is less flexible because it requires a class of proxied object to be visible in
    compilation context
    */
    SECTION("DGateRef direct initialization")
    {
        Functor test;
        DGateRef dgate(test);
        static_assert(std::is_same_v<decltype(dgate), DGateRef<Functor>>);

        REQUIRE(dgate(1) == 1);
        REQUIRE(test.i == 1);

        REQUIRE(dgate("2") == "2");
        REQUIRE(test.s == "2");
    }

    SECTION("DGateRef std::ref initialization")
    {
        Functor test;
        DGateRef dgate(std::ref(test));
        /* Please note:
        The delegate type is DGateRef<Functor> and not
        DGateRef<std::reference_wrapper<Functor>> (as you might expect)
        */
        static_assert(std::is_same_v<decltype(dgate), DGateRef<Functor>>);

        REQUIRE(dgate(1) == 1);
        REQUIRE(test.i == 1);

        REQUIRE(dgate("2") == "2");
        REQUIRE(test.s == "2");
    }
}

struct EventHandler
{
    void operator()(int i)
    {
        this->i = i;
    }
    void operator()(const std::string &s)
    {
        this->s = s;
    }

    int i = 0;
    std::string s;
};

TEST_CASE("EventsDGate", "[core]")
{
    //...note that HandlerDGate has "compatible" interface with EventHandler
    using HandlerDGate = EventsDGate<int, std::string>::Type;
    HandlerDGate dgate;

    SECTION("EventsDGate : predefined DGate type used to proxy event handler")
    {
        EventHandler handler;
        /* Please note:
        Because the type of delegate is define in advance (HandlerDGate),
        we can create an "empty" delegate and then assign a reference of a proxied object to it.
        */
        dgate = std::ref(handler);

        dgate(1);
        REQUIRE(handler.i == 1);

        dgate("2");
        REQUIRE(handler.s == "2");
    }

    SECTION("EventsDGate : using lambda handler")
    {
        int i = 0;
        std::string s;
        /* Please note:
        Here we don't have single EventHandler object,
        however the delegate proxies to something that looks like EventHandler
        */
        dgate = DGate([&i](int j) { i = j; }, [&s](const std::string &str) { s = str; });

        dgate(1);
        REQUIRE(i == 1);

        dgate("2");
        REQUIRE(s == "2");
    }
}
