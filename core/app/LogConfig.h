#pragma once

#include "LogLevel.h"

#include <config/config.h>

#include <optional>
#include <string>

namespace app
{

DEFINE_CONFIG_ENUM(LogLevel, {
                                 {LogLevel::DEBUG, "debug"},
                                 {LogLevel::INFO, "info"},
                                 {LogLevel::WARN, "warn"},
                                 {LogLevel::ERROR, "error"},
                             })

struct LogConfig
{
    LogLevel level = LogLevel::INFO;
    struct File
    {
        std::string name;
        std::optional<std::size_t> rotationSize;
    };
    std::optional<File> file; //... if not set than console
    // TODO: later add format and more rotation rotation options (time, target location, target file pattern)
};

inline void to_json(nlohmann::json &j, const LogConfig::File &fc)
{
    j = nlohmann::json{{"name", fc.name}};
    if (fc.rotationSize)
        j["rotationSize"] = *fc.rotationSize;
}
inline void from_json(const nlohmann::json &j, LogConfig::File &fc)
{
    j.at("name").get_to(fc.name);
    if (j.contains("rotationSize"))
        j.at("rotationSize").get_to(*fc.rotationSize);
}

inline void to_json(nlohmann::json &j, const LogConfig &lc)
{
    j = nlohmann::json{{"level", lc.level}};
    if (lc.file)
        j["file"] = *lc.file;
}
inline void from_json(const nlohmann::json &j, LogConfig &lc)
{
    if (j.contains("level"))
        j.at("level").get_to(lc.level);
    if (j.contains("file"))
        j.at("file").get_to(*lc.file);
}
} // namespace app
