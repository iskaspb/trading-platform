#pragma once
#include <app/configLocator.h>
#include <app/logging.h>

#include <fstream>

namespace app
{
template <typename ConfigT> inline ConfigT init(int argc, char *argv[], const std::string &overrideAppName = {})
{
    using json = nlohmann::json;

    const auto loc = locateConfig(argc, argv, overrideAppName);

    std::string strConfig;
    {
        std::ifstream configFile(loc.path);
        if (!configFile.is_open())
        {
            std::cout << "ERROR: Failed config file from " << loc << std::endl;
            ::exit(1);
        }
        std::stringstream buffer;
        buffer << configFile.rdbuf();
        strConfig = buffer.str();
    }
    ConfigT config = json::parse(strConfig);

    app::initLogger(config.log);
    LOG_DEBUG(loc.appName << " started with " << loc << " : " << strConfig);
    return config;
}
} // namespace app
