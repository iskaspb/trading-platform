#pragma once

#include <boost/program_options.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace app
{
inline std::string locateConfig(int argc, char *argv[], const std::string &overrideAppName = {})
{
    using std::filesystem::path;
    const path appName = path(argv[0]);
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
            return configFile;
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
            return configFile;
        }
    }

    const std::string appConfigName = [&]() {
        if (!overrideAppName.empty())
            return overrideAppName + ".json";
        return appName.filename().string() + ".json";
    }();

    {
        //...check the same directory as the executable ("collocated" config file)
        const path appDir = appName.parent_path();
        const path configFile = appDir / appConfigName;
        if (std::filesystem::exists(configFile))
        {
            return configFile.string();
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
                return configFile.string();
            }
        }
    }

    std::cout << "Config file not found in the following locations : --config, $" << envConfigPath << ", "
              << appName.parent_path() << ", $" << envConfigDir << '/' << appConfigName << std::endl;
    ::exit(1);
}
} // namespace app
