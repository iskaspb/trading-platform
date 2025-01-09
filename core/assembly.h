#pragma once
#include <metal/list/copy_if.hpp>
#include <metal/list/list.hpp>
//#include <metal.hpp>

namespace impl
{
namespace concepts
{
template <typename Node>
concept hasContext = requires()
{
    typename Node::ContextMixin;
};
} // namespace concepts

template <typename T> using hasContext = metal::number<concepts::hasContext<T>>;

template <typename List> struct NodesHolder;
template <typename... Ts> struct NodesHolder<metal::list<Ts...>> : Ts...
{
};

template <typename List> struct ContextHolder;
template <typename... Ts> struct ContextHolder<metal::list<Ts...>> : Ts::ContextMixin...
{
};

template <typename ListT> struct Collector
{
    using ContextNodes = metal::copy_if<ListT, metal::lambda<hasContext>>;
    using Context = ContextHolder<ContextNodes>;
};

template <typename ListT> struct Nodes : Collector<ListT>::Context, NodesHolder<ListT>
{
    using List = ListT;
    using Context = typename Collector<ListT>::Context;
};
} // namespace impl

template <typename TraitsT, template <typename> class... NodesT> struct Assembly
{
    using Traits = TraitsT;
    using Nodes = impl::Nodes<metal::list<NodesT<Traits>...>>;
};
