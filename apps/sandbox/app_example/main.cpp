#include <app/configLocator.h>
#include <app/logging.h>

#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct AppConfig
{
    std::string name;
    app::LogConfig log;
    nlohmann::json strategy;
    friend std::ostream &operator<<(std::ostream &os, const AppConfig &config)
    {
        return os << "AppConfig {name: " << config.name << ", log: " << json(config.log)
                  << ", strategy: " << config.strategy << '}';
    }
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AppConfig, name, log, strategy);

/* Try this example with:

user@29a1e7925970:~/trading-platform/build$ make app_example
[ 50%] Building CXX object apps/sandbox/app_example/CMakeFiles/app_example.dir/main.cpp.o
[100%] Linking CXX executable ../../../bin/app_example
[100%] Built target app_example

user@29a1e7925970:~/trading-platform/build$ ./bin/app_example --config ../apps/sandbox/app_example/app.json
Found config: ../apps/sandbox/app_example/app.json
2025-01-02 07:39:18.416134 INFO  main.cpp:39 App started with config: AppConfig {name: app_example, log:
{"level":"debug"}, strategy: {"name":"mm_binance","params":{"p1":"v1","p2":"v2"}}}
*/
int main(int argc, char *argv[])
{
    //...locate configuration file
    const std::string configPath = app::locateConfig(argc, argv, "app");
    std::cout << "Found config: " << configPath << std::endl;

    //...load config
    std::ifstream configFile(configPath);
    if (!configFile.is_open())
    {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return 1;
    }
    const AppConfig config = nlohmann::json::parse(configFile);

    //...initialize logger and output config
    app::initLogger(config.log);
    LOG_INFO(app::logger::get(), "App started with config: " << config);

    return 0;
}
