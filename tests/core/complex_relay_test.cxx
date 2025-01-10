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

    void onRequest(int &depth, int &count)
    {
        Relay::passResponse(this, depth, count);
    }
};

struct DataService
{
    std::string data;
    mutable std::string prevData;
};

template <typename Traits> struct DataHolder
{
    using ContextMixin = DataService;
};

template <typename Traits> struct DataClientMixin
{
    void update(const std::string &data)
    {
        ctx(this).data = data;
    }

    void update(const std::string &) const
    {
        ctx(this).prevData = ctx(this).data;
    }
};

struct DataEvent
{
    std::string data;
};

template <typename Traits> struct DataClientNode
{
    using ContextMixin = DataClientMixin<Traits>;

    void onRequest(const DataEvent &event)
    {
        //...validate correct context resolution (const/non-const)
        ctx(const_cast<const DataClientNode *>(this)).update(event.data);
        ctx(this).update(event.data);
    }
};

#define D_UNIQ_DUMMY(N)                                                                                                \
    template <typename Traits> struct Dummy##N                                                                         \
    {                                                                                                                  \
    };

D_UNIQ_DUMMY(1)
D_UNIQ_DUMMY(2)
D_UNIQ_DUMMY(3)
D_UNIQ_DUMMY(4)
D_UNIQ_DUMMY(5)
D_UNIQ_DUMMY(6)
D_UNIQ_DUMMY(7)
D_UNIQ_DUMMY(8)
D_UNIQ_DUMMY(9)
D_UNIQ_DUMMY(10)
D_UNIQ_DUMMY(11)
D_UNIQ_DUMMY(12)

#define D_UNIQ_NODE(N)                                                                                                 \
    template <typename Traits> struct Node##N                                                                          \
    {                                                                                                                  \
        using Relay = typename core::Relay<Traits>;                                                                    \
                                                                                                                       \
        void onRequest(const Message &msg)                                                                             \
        {                                                                                                              \
            Relay::passRequest(this, msg);                                                                             \
        }                                                                                                              \
                                                                                                                       \
        void onResponse(const Message &msg)                                                                            \
        {                                                                                                              \
            Relay::passResponse(this, msg);                                                                            \
        }                                                                                                              \
                                                                                                                       \
        void onRequest(int &depth, int &count)                                                                         \
        {                                                                                                              \
            ++depth;                                                                                                   \
            ++count;                                                                                                   \
            Relay::passRequest(this, depth, count);                                                                    \
        }                                                                                                              \
                                                                                                                       \
        void onResponse(int &depth, int &count)                                                                        \
        {                                                                                                              \
            --depth;                                                                                                   \
            ++count;                                                                                                   \
            Relay::passResponse(this, depth, count);                                                                   \
        }                                                                                                              \
    };

D_UNIQ_NODE(1)
D_UNIQ_NODE(2)
D_UNIQ_NODE(3)
D_UNIQ_NODE(4)
D_UNIQ_NODE(5)
D_UNIQ_NODE(6)
D_UNIQ_NODE(7)
D_UNIQ_NODE(8)
D_UNIQ_NODE(9)

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

struct TestAssembly : core::Assembly<TestAssembly, DataHolder, Dummy1, core::ResponseTerminator, Dummy2, Dummy3, Node1,
                                     Dummy4, Node2, Dummy5, Node3, Dummy6, Node4, Dummy7, Node5, Dummy8, Node6, Dummy9,
                                     Node7, Dummy10, Node8, Dummy11, Node9, Echo, Dummy12, DataClientNode>
{
    using Acceptor = ::Acceptor;
};

TEST_CASE("ComplexRelay", "[core]")
{
    core::Relay<TestAssembly> relay;
    Acceptor acceptor;
    relay.request(acceptor);
    // relay.respond(acceptor);

    SECTION("Echo event acceptor")
    {
        /*
        This test validates a workflow where an event is sent to the first node in the relay
        and the event is processed by the nodes in the relay and finally accepted by the acceptor.
        */
        relay.request(Message{"hello"});
        relay.request(Message{"world"});

        REQUIRE(acceptor.messages.size() == 2);
        CHECK(acceptor.messages[0].data == "olleh");
        CHECK(acceptor.messages[1].data == "dlrow");
    }

    SECTION("Echo transaction processing")
    {
        /*
        This test validates a workflow where a transaction is sent to the first node in the relay
        and the transaction is processed by the nodes in the relay and finally accepted by the acceptor.
        The difference with the previous test is that data is updated by the nodes.
        Also note that some nodes (dummy, ...) do not participate in the transaction processing.
        */
        int depth = 0, count = 0;
        relay.request(depth, count);
        CHECK(depth == 0);  // final depth = 0 because the transaction returned
        CHECK(count == 18); // 9 nodes * 2 (all nodes * 2)

        CHECK(acceptor.messages.empty());
    }

    SECTION("ContextMixin client/service")
    {
        /*
        This test validates communication between data client mixin and data service mixin.
        Note, that the context is correctly resolved for the const and non-const methods.
        */
        relay.request(DataEvent{"some data"});
        CHECK(relay.ctx().prevData == "");
        CHECK(relay.ctx().data == "some data");

        relay.request(DataEvent{"new data"});
        CHECK(relay.ctx().prevData == "some data");
        CHECK(relay.ctx().data == "new data");

        CHECK(acceptor.messages.empty());
    }
}
