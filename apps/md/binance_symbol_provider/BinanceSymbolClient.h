#pragma once
#include "SymbolProviderConfig.h"

#include <app/logging.h>
#include <assembly/relay.h>
#include <io/HTTPS.h>
#include <market/Events.h>

enum class ServiceCmd
{
    Start,
    Stop
};

template <typename Traits> struct BinanceSymbolClient
{
    using Relay = typename core::Relay<Traits>;

    template <HasSymbolProviderConfig Config>
    explicit BinanceSymbolClient(Config &config)
        : address_(config.symbolProvider.source), client_(net::make_strand(core::ctx(this).getIO()), address_)
    {
        LOG_DEBUG("BinanceSymbolClient initialized with " << config.symbolProvider.source);
    }

    void onRequest(ServiceCmd command)
    {
        switch (command)
        {
        case ServiceCmd::Start: {
            LOG_DEBUG("BinanceSymbolClient requests symbols from " << address_.target());
            HTTPS::Response response = client_.send(HTTPS::Get(address_.target()));
            LOG_DEBUG("Response: code:" << response.code);
            const SymbolData symbols{.data = std::move(response.body)};
            Relay::passResponse(this, symbols);
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
