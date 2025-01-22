#pragma once
#include "assembly.h"

#include <metal/lambda.hpp>
#include <metal/list/drop.hpp>
#include <metal/list/find.hpp>
#include <metal/list/find_if.hpp>
#include <metal/list/reverse.hpp>

namespace core
{
namespace impl
{
namespace concepts
{
template <typename Node, typename... Msgs>
concept hasOnRequest = requires(Node *node, Msgs... msgs)
{
    node->onRequest(msgs...);
};

template <typename Node, typename... Msgs>
concept hasOnResponse = requires(Node *node, Msgs... msgs)
{
    node->onResponse(msgs...);
};
} // namespace concepts

template <typename Node, typename... Msgs> using hasOnRequest = metal::number<concepts::hasOnRequest<Node, Msgs...>>;
template <typename Node, typename... Msgs> using hasOnResponse = metal::number<concepts::hasOnResponse<Node, Msgs...>>;
template <typename... Msgs>
using hasOnRequestCond = metal::bind<metal::lambda<hasOnRequest>, metal::_1, metal::always<Msgs>...>;
template <typename... Msgs>
using hasOnResponseCond = metal::bind<metal::lambda<hasOnResponse>, metal::_1, metal::always<Msgs>...>;

} // namespace impl

template <typename AssemblyT> struct Relay
{
    using Assembly = AssemblyT;
    using Context = typename Assembly::Body::Context;
    using Nodes = typename Assembly::Nodes;
    using RNodes = metal::reverse<Nodes>;
    static_assert(std::is_same_v<typename Assembly::Traits, Assembly>, "Traits must be derived from Assembly<...>");

    Relay() = default;
    template <typename... Args> Relay(Args &&...args) : assembly_(std::forward<Args>(args)...)
    {
    }

    Context &ctx() noexcept
    {
        return assembly_.ctx();
    }
    const Context &ctx() const noexcept
    {
        return assembly_.ctx();
    }

    template <typename... Msgs> void request(Msgs &&...msgs)
    {
        using NodePos = metal::find_if<Nodes, impl::hasOnRequestCond<Msgs...>>;
        static_assert(NodePos::value < Assembly::size, "No node found with onRequest(Msgs...)");
        using Node = metal::at<Nodes, NodePos>;

        static_cast<Node *>(&assembly_)->onRequest(std::forward<Msgs>(msgs)...);
    }
    template <typename... Msgs> void respond(Msgs &&...msgs)
    {
        using NodePos = metal::find_if<RNodes, impl::hasOnResponseCond<Msgs...>>;
        static_assert(NodePos::value < Assembly::size, "No node found with onResponse(Msgs...)");
        using Node = metal::at<RNodes, NodePos>;

        static_cast<Node *>(&assembly_)->onResponse(std::forward<Msgs>(msgs)...);
    }

    template <typename Node, typename... Msgs> static void passRequest(Node *node, Msgs &&...msgs)
    {
        using CurNodePos = metal::find<Nodes, Node>;
        using RemainingNodes = metal::drop<Nodes, metal::number<CurNodePos::value + 1>>;
        using NodePos = metal::find_if<RemainingNodes, impl::hasOnRequestCond<Msgs...>>;
        static_assert(NodePos::value < metal::size<RemainingNodes>::value,
                      "No next node found with onRequest(Msgs...)");
        using NextNode = metal::at<RemainingNodes, NodePos>;

        static_cast<NextNode *>(static_cast<AssemblyBody *>(node))->onRequest(std::forward<Msgs>(msgs)...);
    }

    template <typename Node, typename... Msgs> static void passResponse(Node *node, Msgs &&...msgs)
    {
        using CurNodePos = metal::find<RNodes, Node>;
        using RemainingNodes = metal::drop<RNodes, metal::number<CurNodePos::value + 1>>;
        using NodePos = metal::find_if<RemainingNodes, impl::hasOnResponseCond<Msgs...>>;
        static_assert(NodePos::value < metal::size<RemainingNodes>::value,
                      "No next node found with onResponse(Msgs...)");
        using NextNode = metal::at<RemainingNodes, NodePos>;

        static_cast<NextNode *>(static_cast<AssemblyBody *>(node))->onResponse(std::forward<Msgs>(msgs)...);
    }

  private:
    using AssemblyBody = typename Assembly::Body;
    AssemblyBody assembly_;
};
} // namespace core