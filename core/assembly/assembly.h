#pragma once
#include <metal/list/copy_if.hpp>
#include <metal/list/list.hpp>
#include <type_traits>
#include <utility>

namespace core
{
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

template <typename Nodes> struct ContructibleNodes;
template <typename... Nodes> struct ContructibleNodes<metal::list<Nodes...>> : Constructible<Nodes>...
{
    template <typename... Args> ContructibleNodes(const Args &...args) : Constructible<Nodes>(args...)...
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
template <typename... Ts> struct ContextHolder<metal::list<Ts...>> : Constructible<typename Ts::ContextMixin>...
{
    template <typename... Args>
    ContextHolder(const Args &...args) : Constructible<typename Ts::ContextMixin>(args...)...
    {
    }
};

template <typename Nodes> struct ContextCollector
{
    using ContextNodes = metal::copy_if<Nodes, metal::lambda<hasContext>>;
    using Context = ContextHolder<ContextNodes>;
};

template <typename Nodes> struct Assembly : ContextCollector<Nodes>::Context, ContructibleNodes<Nodes>
{
    using Context = typename ContextCollector<Nodes>::Context;

    template <typename... Args> Assembly(const Args &...args) : Context(args...), ContructibleNodes<Nodes>(args...)
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

template <typename NodeOrCtxMixin> auto &getAssembly(NodeOrCtxMixin *node_or_mixin);

template <template <typename> class Node, typename AssemblyT> inline AssemblyT::Body &getAssembly(Node<AssemblyT> *node)
{
    return static_cast<AssemblyT::Body &>(*node);
}

template <template <typename> class Node, typename AssemblyT>
inline const AssemblyT::Body &getAssembly(const Node<AssemblyT> *node)
{
    return static_cast<const AssemblyT::Body &>(*node);
}

} // namespace impl

template <typename TraitsT, template <typename> class... NodesT> struct Assembly
{
    using Traits = TraitsT;
    using Nodes = metal::list<NodesT<Traits>...>;
    static constexpr size_t size = metal::size<Nodes>::value;
    using Body = impl::Assembly<Nodes>;
};

template <typename NodeOrCtxMixin> inline auto &ctx(NodeOrCtxMixin *node_or_mixin)
{
    return impl::getAssembly(node_or_mixin).ctx();
}
} // namespace core