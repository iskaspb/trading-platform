#pragma once
#include <string>

struct OnConnected
{
    uint64_t ts{};
};

struct OnDisconnected
{
    uint64_t ts{};
    std::string reason;
};
