#include "AppConfig.h"

#include <app/init.h>
#include <assembly/relay.h>
#include <io/AsioService.h>

template <typename Traits> struct FileMDStore
{
};

template <typename Traits> struct BinanceMDClient
{
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
        // request(ServiceCmd::Start);
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
