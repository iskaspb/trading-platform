#pragma once

template <typename... Callable> struct DGate : Callable...
{
    using Callable::operator()...;
    constexpr DGate(Callable... c) : Callable(std::move(c))...
    {
    }
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
