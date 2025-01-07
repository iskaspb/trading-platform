#pragma once

#include <util/enum.h>

#include <boost/program_options.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace app
{
ENUM_CLASS(ConfigLocType, NONE, CMD_LINE_CONFIG, ENV_CONFIG, COLLOCATED, ENV_CONFIG_DIR);
struct ConfigLocation
{
    using enum ConfigLocType;
    std::string appName, path;
    ConfigLocType locType{NONE};

    friend std::ostream &operator<<(std::ostream &os, const ConfigLocation &cl)
    {
        return os << "ConfigLocation {appName: " << cl.appName << ", locType: " << cl.locType << ", path: " << cl.path
                  << '}';
    }
};

namespace impl
{
using std::filesystem::path;
namespace po = boost::program_options;
static constexpr char envConfigPath[] = "APP_CONFIG";
static constexpr char envConfigDir[] = "APP_CONFIG_DIR";

inline bool locateConfigImpl(const path &appPath, ConfigLocation &loc)
{
    {
        //...check if ENV is present and use it as a config file path
        const auto configFile = std::getenv(envConfigPath);
        if (configFile)
        {
            if (!std::filesystem::exists(configFile))
            {
                std::cout << "Using " << envConfigPath << " env variable, ERROR: Config file " << configFile
                          << " is not accessible" << std::endl;
                ::exit(1);
            }
            loc.locType = ConfigLocation::ENV_CONFIG;
            loc.path = configFile;
            return true;
        }
    }

    const std::string appConfigName = loc.appName + ".json";
    {
        //...check the same directory as the executable ("collocated" config file)
        const path appDir = appPath.parent_path();
        const path configFile = appDir / appConfigName;
        if (std::filesystem::exists(configFile))
        {
            loc.locType = ConfigLocation::COLLOCATED;
            loc.path = configFile.string();
            return true;
        }
    }

    {
        //...check if ENV is present and use it as a location for config file
        const auto configDir = std::getenv(envConfigDir);
        if (configDir)
        {
            path configFile = path(configDir) / appConfigName;
            if (std::filesystem::exists(configFile))
            {
                loc.locType = ConfigLocation::ENV_CONFIG_DIR;
                loc.path = configFile.string();
                return true;
            }

            configFile = path(configDir) / loc.appName / appConfigName;
            if (std::filesystem::exists(configFile))
            {
                loc.locType = ConfigLocation::ENV_CONFIG_DIR;
                loc.path = configFile.string();
                return true;
            }
        }
    }
    return false;
}
} // namespace impl

inline ConfigLocation locateConfig(int argc, char *argv[], const std::string &overrideAppName = {})
{
    using namespace impl;

    const path appPath = path(argv[0]);
    ConfigLocation loc;
    loc.appName = overrideAppName.empty() ? appPath.filename().string() : overrideAppName;
    {
        //...check if --config is present and use it as a config file path
        po::variables_map vm;
        po::options_description desc{appPath.filename().string() + " options"};
        desc.add_options()("help,h", "Print help")("config,c", po::value<std::string>(), "Config file");

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            ::exit(0);
        }

        if (vm.count("config"))
        {
            const auto configFile = vm["config"].as<std::string>();
            if (!std::filesystem::exists(configFile))
            {
                std::cout << "Using filename provided by --config, ERROR: Config file  " << configFile
                          << " is not accessible" << std::endl;
                ::exit(1);
            }
            loc.locType = ConfigLocation::CMD_LINE_CONFIG;
            loc.path = configFile;
            return loc;
        }
    }

    if (locateConfigImpl(appPath, loc))
        return loc;

    std::cout << "Config file not found in the following locations : --config, $" << envConfigPath << ", "
              << appPath.parent_path() << ", $" << envConfigDir << "/[" << loc.appName << "]/" << loc.appName << ".json"
              << std::endl;
    ::exit(1);
}

inline ConfigLocation locateConfig(const std::string &exePath, const std::string &overrideAppName = {})
{
    using namespace impl;

    const path appPath = path(exePath);
    ConfigLocation loc;
    loc.appName = overrideAppName.empty() ? appPath.filename().string() : overrideAppName;

    if (locateConfigImpl(appPath, loc))
        return loc;

    std::cout << "Config file not found in the following locations : $" << envConfigPath << ", "
              << appPath.parent_path() << ", $" << envConfigDir << "/[" << loc.appName << "]/" << loc.appName << ".json"
              << std::endl;
    ::exit(1);
}

} // namespace app
