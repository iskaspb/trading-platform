#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>

template <typename Traits> struct FileSymbolStore
{
    template <HasSymbolProviderConfig Config> explicit FileSymbolStore(Config &config)
    {
        LOG_DEBUG("FileSymbolStore will store data into " << config.symbolProvider.store);
    }

    void onResponse(const SymbolData &symbols)
    {
        LOG_DEBUG("FileSymbolStore stores " << symbols.data.substr(0, 100) << "...");
        //...add code to store symbols into a file
    }
};
