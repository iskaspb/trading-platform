#pragma once
#include <app/configLocator.h>
#include <app/logging.h>

#include <fstream>

namespace app
{
namespace impl
{
template <typename ConfigT> inline ConfigT init(const ConfigLocation &loc)
{
    using json = nlohmann::json;

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
} // namespace impl

template <typename ConfigT> inline ConfigT init(int argc, char *argv[], const std::string &overrideAppName = {})
{
    return impl::init<ConfigT>(locateConfig(argc, argv, overrideAppName));
}

template <typename ConfigT> inline ConfigT init(const std::string &exePath, const std::string &overrideAppName = {})
{
    return impl::init<ConfigT>(locateConfig(exePath, overrideAppName));
}

inline void init(const LogConfig &logConfig = {})
{
    app::initLogger(logConfig);
    LOG_DEBUG("Application has started");
}
} // namespace app
