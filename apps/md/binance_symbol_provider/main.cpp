#include <AppConfig.h>

#include <app/init.h>
#include <assembly/relay.h>

template <typename Traits> struct AsioService
{
};

enum class ServiceCommand
{
    Start,
    Stop
};

template <typename Traits> struct BinanceSymbolClient
{
    template <HasSymbolProviderConfig Config> explicit BinanceSymbolClient(Config &)
    {
    }

    void onRequest(ServiceCommand command)
    {
    }
};

template <typename Traits> struct FileSymbolStore
{
    template <HasSymbolProviderConfig Config> explicit FileSymbolStore(Config &)
    {
    }
};

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
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
    }
}
