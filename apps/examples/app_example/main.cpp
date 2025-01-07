#include <app/init.h>

#include <iostream>

using json = nlohmann::json;

struct AppConfig
{
    std::string name;
    app::LogConfig log;
    json strategy;
    friend std::ostream &operator<<(std::ostream &os, const AppConfig &config)
    {
        return os << "AppConfig {name: " << config.name << ", log: " << json(config.log)
                  << ", strategy: " << config.strategy << '}';
    }
};
DEFINE_CONFIG(AppConfig, name, log, strategy);

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
    auto config = app::init<AppConfig>(argc, argv, "app");

    return 0;
}
