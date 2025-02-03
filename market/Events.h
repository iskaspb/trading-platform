#pragma once
#include <io/Events.h>

struct SymbolData
{
    uint64_t ts{};
    std::string data;
};

struct MarketData
{
    uint64_t ts{};
    std::string data;
};
