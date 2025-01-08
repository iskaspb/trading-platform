#pragma once
#include <functional>

template <typename... Callables> struct DGate : Callables...
{
    using Callables::operator()...;
    DGate() = default;
    constexpr DGate(Callables... c) : Callables(std::move(c))...
    {
    }
    template <typename Callable> constexpr DGate(Callable c) : Callables(c)...
    {
    }
};

template <typename... Callables> DGate(Callables...) -> DGate<Callables...>;
template <typename Callable> DGate(Callable) -> DGate<Callable>;

template <typename... Events> struct EventsDGate
{
    using Type = DGate<std::function<void(Events)>...>;
};

template <typename Callable> class DGateRef
{
  public:
    DGateRef(Callable &c) : ref_(c)
    {
    }
    DGateRef(const std::reference_wrapper<Callable> &ref) : ref_(ref)
    {
    }

    template <typename... Args> auto operator()(Args &&...args)
    {
        return ref_(std::forward<Args>(args)...);
    }

  private:
    const std::reference_wrapper<Callable> ref_;
};
