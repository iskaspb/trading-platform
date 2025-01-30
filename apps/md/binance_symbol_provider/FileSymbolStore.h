#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>
#include <assembly/assembly.h>

template <typename Traits> struct FileSymbolStore
{
    template <HasSymbolProviderConfig Config> explicit FileSymbolStore(Config &config)
    {
        LOG_DEBUG(core::ctx(this).log(), "FileSymbolStore will store data into " << config.symbolProvider.store);
    }

    void onResponse(const SymbolData &symbols)
    {
        LOG_DEBUG(core::ctx(this).log(), "FileSymbolStore stores " << symbols.data.substr(0, 100) << "...");
        //...add code to store symbols into a file
    }
};
