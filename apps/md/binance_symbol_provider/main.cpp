#include "AppConfig.h"
#include "AsioService.h"
#include "BinanceSymbolClient.h"
#include "FileSymbolStore.h"
#include "LoggerNode.h"

#include <app/init.h>
#include <assembly/relay.h>

struct SymbolProviderAsm
    : core::Assembly<SymbolProviderAsm, LoggerNode, AsioService, FileSymbolStore, BinanceSymbolClient>
{
    using Config = AppConfig;
};

struct SymbolProvider : core::Relay<SymbolProviderAsm>
{
    explicit SymbolProvider(const AppConfig &config) : core::Relay<SymbolProviderAsm>(config)
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

        SymbolProvider symbolProvider(config);
        symbolProvider.start();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
    }
}
