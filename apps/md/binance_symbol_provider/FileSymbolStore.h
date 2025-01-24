#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>

template <typename Traits> struct FileSymbolStore
{
    template <HasSymbolProviderConfig Config> explicit FileSymbolStore(Config &config)
    {
        LOG_DEBUG("FileSymbolStore will store into " << config.symbolProvider.store);
    }
};
