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

int main(int argc, char *argv[])
{
    try
    {
        const auto config = app::init<AppConfig>(argc, argv);

        core::Relay<Assembly> relay(config);

        relay.request(ServiceCmd::Start);

        relay.ctx().getIO().run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
    }
}
