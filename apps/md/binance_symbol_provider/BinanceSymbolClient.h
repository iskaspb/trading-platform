#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>
#include <assembly/relay.h>
#include <io/HTTPS.h>

enum class ServiceCmd
{
    Start,
    Stop
};

template <typename Traits> struct BinanceSymbolClient
{
    template <HasSymbolProviderConfig Config>
    explicit BinanceSymbolClient(Config &config)
        : address_(config.symbolProvider.source), client_(net::make_strand(core::ctx(this).getIO()), address_)
    {
        LOG_DEBUG("BinanceSymbolClient gets symbols from " << config.symbolProvider.source);
    }

    void onRequest(ServiceCmd command)
    {
        switch (command)
        {
        case ServiceCmd::Start: {
            LOG_DEBUG("BinanceSymbolClient started");
            const auto response = client_.send(HTTPS::Get(address_.target()));
            LOG_DEBUG("Response: code:" << response.code << "\n" << response.body);
            break;
        }
        case ServiceCmd::Stop:
            LOG_DEBUG("BinanceSymbolClient stopped");
            break;
        }
    }

  private:
    URL address_;
    HTTPS client_;
};
