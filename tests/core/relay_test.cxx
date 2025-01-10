#include <catch2/catch_all.hpp>
#include <relay.h>

namespace core
{
template <typename Traits> struct ResponseTerminator
{
    struct ContextMixin
    {
        Traits::Acceptor *acceptor{};
    };

    void onRequest(Traits::Acceptor &a)
    {
        core::ctx(this).acceptor = &a;
    }

    template <typename... Args> void onResponse(Args &&...args)
    {
        assert(core::ctx(this).acceptor && "Set acceptor before responding");
        core::ctx(this).acceptor->accept(std::forward<Args>(args)...);
    }
};
} // namespace core

using core::ctx;

struct Message
{
    std::string data;

    friend std::ostream &operator<<(std::ostream &os, const Message &m)
    {
        os << m.data;
        return os;
    }
};

template <typename Traits> struct Echo
{
    using Relay = typename core::Relay<Traits>;

    void onRequest(const Message &msg)
    {
        std::string reply(msg.data);
        std::reverse(reply.begin(), reply.end());
        Relay::passResponse(this, Message{reply});
    }
};

struct Acceptor
{
    std::vector<Message> messages;

    void accept(const Message &msg)
    {
        messages.push_back(msg);
    }

    void accept(int, int)
    {
    }
};

struct TestAssembly : core::Assembly<TestAssembly, core::ResponseTerminator, Echo>
{
    using Acceptor = ::Acceptor;
};

TEST_CASE("Relay", "[core]")
{
    core::Relay<TestAssembly> relay;
    Acceptor acceptor;
    relay.request(acceptor);

    SECTION("Echo event acceptor")
    {
        relay.request(Message{"hello"});
        relay.request(Message{"world"});

        REQUIRE(acceptor.messages.size() == 2);
        CHECK(acceptor.messages[0].data == "olleh");
        CHECK(acceptor.messages[1].data == "dlrow");
    }
}
