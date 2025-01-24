#include "AppConfig.h"
#include "AsioService.h"
#include "BinanceSymbolClient.h"
#include "FileSymbolStore.h"

#include <app/init.h>
#include <assembly/relay.h>

struct Assembly : core::Assembly<Assembly, AsioService, FileSymbolStore, BinanceSymbolClient>
{
    using Config = AppConfig;
};

struct SymbolProvider : core::Relay<Assembly>
{
    explicit SymbolProvider(const AppConfig &config) : core::Relay<Assembly>(config)
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
