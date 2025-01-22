#include <app/init.h>

struct AppConfig
{
    app::LogConfig log;
    nlohmann::json other;
};
DEFINE_CONFIG(AppConfig, log, other);

int main(int argc, char *argv[])
{
    const auto config = app::init<AppConfig>(argc, argv);
    //...add your code here
    LOG_INFO("Hello, binance_md_capture!");
}
