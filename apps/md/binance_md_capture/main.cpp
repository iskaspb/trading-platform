#include "AppConfig.h"

#include <app/init.h>
#include <assembly/relay.h>
#include <io/AsioService.h>
#include <io/WSS.h>
#include <market/Events.h>

template <typename Traits> struct FileMDStore
{
    template <HasMDCaptureConfig Config> explicit FileMDStore(Config &config)
    {
        LOG_DEBUG("FileMDStore will store data into " << config.mdCapture.fileStore);
    }

    void onResponse(const MarketData &md)
    {
        LOG_DEBUG("FileMDStore stores " << md.data);
        //...store into a file...
    }
};

enum class ServiceCmd
{
    Start,
    Stop
};

//...https://developers.binance.com/docs/binance-spot-api-docs/web-socket-api/general-api-information
template <typename Traits> struct BinanceMDClient
{
    using Relay = typename core::Relay<Traits>;

    template <HasMDCaptureConfig Config>
    explicit BinanceMDClient(Config &config)
        : wss_(std::ref(*this), net::make_strand(core::ctx(this).getIO()), URL(config.mdCapture.address)),
          symbols_(config.mdCapture.symbols)
    {
        LOG_DEBUG("BinanceMDClient gets MD from " << config.mdCapture.address);
        if (symbols_.empty())
            throw std::runtime_error("No symbols provided");
    }

    void onRequest(ServiceCmd command)
    {
        switch (command)
        {
        case ServiceCmd::Start: {
            wss_.connect();
            break;
        }
        case ServiceCmd::Stop:
            LOG_DEBUG("BinanceSymbolClient stopped");
            break;
        }
    }

    void operator()(WSS &ws, const OnConnected &)
    {
        LOG_DEBUG("WS connected to " << ws.host() << ':' << ws.port() << "; sending subscription request...");
        ws.send(std::format(R"json({{"method": "SUBSCRIBE","params": ["{}@trade"],"id": 1}})json", symbols_[0]));
        // ws.send(R"json({"method": "SUBSCRIBE","params": ["btcusdt@trade"],"id": 1})json");
    }

    void operator()(WSS &ws, const OnDisconnected &)
    {
        LOG_DEBUG("WS disconnected from " << ws.host() << ':' << ws.port());
    }

    void operator()(WSS &, const WSData &wsd)
    {
        Relay::passResponse(this, MarketData{.ts = wsd.ts, .data = wsd.data});
    }

  private:
    WSS wss_;
    std::vector<std::string> symbols_;
};

struct BinanceMDCaptureAsm : core::Assembly<BinanceMDCaptureAsm, AsioService, FileMDStore, BinanceMDClient>
{
    using Config = AppConfig;
};

struct App : core::Relay<BinanceMDCaptureAsm>
{
    explicit App(const AppConfig &config) : core::Relay<BinanceMDCaptureAsm>(config)
    {
    }

    void start()
    {
        request(ServiceCmd::Start);
        ctx().getIO().run();
    }
};

int main(int argc, char *argv[])
{
    try
    {
        const auto config = app::init<AppConfig>(argc, argv);

        App(config).start();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
    }
}
