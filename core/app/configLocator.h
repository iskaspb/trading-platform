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

inline ConfigLocation locateConfig(int argc, char *argv[], const std::string &overrideAppName = {})
{
    using std::filesystem::path;
    const path appName = path(argv[0]);
    ConfigLocation result;
    result.appName = overrideAppName.empty() ? appName.filename().string() : overrideAppName;
    {
        //...check if --config is present and use it as a config file path
        namespace po = boost::program_options;

        po::variables_map vm;
        po::options_description desc{appName.filename().string() + " options"};
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
            result.locType = ConfigLocation::CMD_LINE_CONFIG;
            result.path = configFile;
            return result;
        }
    }

    static constexpr char envConfigPath[] = "APP_CONFIG";
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
            result.locType = ConfigLocation::ENV_CONFIG;
            result.path = configFile;
            return result;
        }
    }

    const std::string appConfigName = result.appName + ".json";
    {
        //...check the same directory as the executable ("collocated" config file)
        const path appDir = appName.parent_path();
        const path configFile = appDir / appConfigName;
        if (std::filesystem::exists(configFile))
        {
            result.locType = ConfigLocation::COLLOCATED;
            result.path = configFile.string();
            return result;
        }
    }

    static constexpr char envConfigDir[] = "APP_CONFIG_DIR";
    {
        //...check if ENV is present and use it as a location for config file
        const auto configDir = std::getenv(envConfigDir);
        if (configDir)
        {
            const path configFile = path(configDir) / appConfigName;
            if (std::filesystem::exists(configFile))
            {
                result.locType = ConfigLocation::ENV_CONFIG_DIR;
                result.path = configFile.string();
                return result;
            }
        }
    }

    std::cout << "Config file not found in the following locations : --config, $" << envConfigPath << ", "
              << appName.parent_path() << ", $" << envConfigDir << '/' << appConfigName << std::endl;
    ::exit(1);
}
} // namespace app
