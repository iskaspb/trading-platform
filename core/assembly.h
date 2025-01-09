#pragma once
#include <metal/list/copy_if.hpp>
#include <metal/list/list.hpp>
//#include <metal.hpp>

namespace impl
{
template <typename Node> struct Constructible : Node
{
    template <typename... Args>
    requires(std::is_constructible_v<Node, Args...>) Constructible(Args &&...args) : Node(std::forward<Args>(args)...)
    {
    }
    template <typename... Args>
    requires(!std::is_constructible_v<Node, Args...>) Constructible(Args &&...)
    {
    }
};

template <typename Nodes> struct NodeHolder;
template <typename... Nodes> struct NodeHolder<metal::list<Nodes...>> : Constructible<Nodes>...
{
    template <typename... Args> NodeHolder(Args &&...args) : Constructible<Nodes>(std::forward<Args>(args)...)...
    {
    }
};

namespace concepts
{
template <typename Node>
concept hasContext = requires()
{
    typename Node::ContextMixin;
};
} // namespace concepts
template <typename T> using hasContext = metal::number<concepts::hasContext<T>>;

template <typename List> struct ContextHolder;
template <typename... Ts> struct ContextHolder<metal::list<Ts...>> : Ts::ContextMixin...
{
};

template <typename Nodes> struct Collector
{
    using ContextNodes = metal::copy_if<Nodes, metal::lambda<hasContext>>;
    using Context = ContextHolder<ContextNodes>;
};

template <typename Context, typename Nodes> struct AssemblyHolder : Context, NodeHolder<Nodes>
{
    template <typename... Args> AssemblyHolder(Args &&...args) : NodeHolder<Nodes>(std::forward<Args>(args)...)
    {
    }

    Context &ctx() noexcept
    {
        return *static_cast<Context *>(this);
    }
    const Context &ctx() const noexcept
    {
        return *static_cast<const Context *>(this);
    }
    template <size_t I> auto &get()
    {
        using Node = metal::at<Nodes, metal::number<I>>;
        return *static_cast<Node *>(this);
    }
};
} // namespace impl

template <typename TraitsT, template <typename> class... NodesT> struct Assembly
{
    using Traits = TraitsT;
    using Nodes = metal::list<NodesT<Traits>...>;
    using Context = typename impl::Collector<Nodes>::Context;
    using Holder = impl::AssemblyHolder<Context, Nodes>;
};
