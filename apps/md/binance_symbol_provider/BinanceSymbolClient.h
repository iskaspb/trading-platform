#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>

enum class ServiceCmd
{
    Start,
    Stop
};

template <typename Traits> struct BinanceSymbolClient
{
    template <HasSymbolProviderConfig Config> explicit BinanceSymbolClient(Config &config)
    {
        LOG_DEBUG("BinanceSymbolClient gets symbols from " << config.symbolProvider.source);
    }

    void onRequest(ServiceCmd command)
    {
        switch (command)
        {
        case ServiceCmd::Start:
            LOG_DEBUG("BinanceSymbolClient started");
            break;
        case ServiceCmd::Stop:
            LOG_DEBUG("BinanceSymbolClient stopped");
            break;
        }
    }
};
