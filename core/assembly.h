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

template <typename Nodes> struct NodeHolder;
template <typename... Nodes> struct NodeHolder<metal::list<Nodes...>> : Constructible<Nodes>...
{
    template <typename... Args> NodeHolder(const Args &...args) : Constructible<Nodes>(args...)...
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

/* This code is simpler than manually written context mixing collector, but it doesn't work due to circular dependency

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

The intended use:
AssemblyHolder inherits from Collector<Nodes>::Context instead of MixinCollector<...

TODO: review it after some time and try to make it work
*/

template <typename... Nodes> struct ContextMixins;

template <typename Node, typename... Rest>
requires(concepts::hasContext<Node>) struct ContextMixins<Node, Rest...>
    : Constructible<typename Node::ContextMixin>, ContextMixins<Rest...>
{
    using Mixin = Constructible<typename Node::ContextMixin>;
    using RestMixins = ContextMixins<Rest...>;

    template <typename... Args> ContextMixins(const Args &...args) : Mixin(args...), RestMixins(args...)
    {
    }
};

template <typename Node, typename... Rest>
requires(!concepts::hasContext<Node>) struct ContextMixins<Node, Rest...> : ContextMixins<Rest...>
{
    using RestMixins = ContextMixins<Rest...>;

    template <typename... Args> ContextMixins(const Args &...args) : RestMixins(args...)
    {
    }
};

template <> struct ContextMixins<>
{
    template <typename... Args> ContextMixins(const Args &...)
    {
    }
};

template <typename AssemblyHolderT> struct MixinCollector;
template <template <typename...> class AssemblyHolderT, typename... Nodes>
struct MixinCollector<AssemblyHolderT<metal::list<Nodes...>>>
{
    template <template <typename...> class NodesToMixins> using Mixins = NodesToMixins<Nodes...>;
};

template <typename Nodes>
struct AssemblyHolder : MixinCollector<AssemblyHolder<Nodes>>::template Mixins<ContextMixins>, NodeHolder<Nodes>
{
    using Context = typename MixinCollector<AssemblyHolder>::template Mixins<ContextMixins>;

    template <typename... Args> AssemblyHolder(const Args &...args) : Context(args...), NodeHolder<Nodes>(args...)
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

template <typename NodeOrCtxMixin> auto &getAssemblyHolder(NodeOrCtxMixin *node_or_mixin);

template <template <typename> class Node, typename AssemblyT>
inline AssemblyT::Holder &getAssemblyHolder(Node<AssemblyT> *node)
{
    return static_cast<AssemblyT::Holder &>(*node);
}

template <template <typename> class Node, typename AssemblyT>
inline const AssemblyT::Holder &getAssemblyHolder(const Node<AssemblyT> *node)
{
    return static_cast<const AssemblyT::Holder &>(*node);
}
} // namespace impl

template <typename TraitsT, template <typename> class... NodesT> struct Assembly
{
    using Traits = TraitsT;
    using Nodes = metal::list<NodesT<Traits>...>;
    static constexpr size_t size = metal::size<Nodes>::value;
    using Holder = impl::AssemblyHolder<Nodes>;
};

template <typename NodeOrCtxMixin> inline auto &ctx(NodeOrCtxMixin *node_or_mixin)
{
    return impl::getAssemblyHolder(node_or_mixin).ctx();
}
} // namespace core