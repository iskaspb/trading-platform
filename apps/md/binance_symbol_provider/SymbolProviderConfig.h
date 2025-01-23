#pragma once

#include <config/config.h>

struct SymbolProviderConfig
{
    nlohmann::json store, source;
};

DEFINE_CONFIG(SymbolProviderConfig, store, source);

template <typename Config>
concept HasSymbolProviderConfig = requires(Config config)
{
    config.symbolProvider;
};
