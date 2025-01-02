#pragma once

#include <config/config.h>
#include <util/enum.h>

// TODO: think how to put all boost::log includes into precompiled headers
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <iosfwd>

namespace app
{
namespace log
{
using namespace boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
} // namespace log

ENUM_CLASS(LogLevel, DEBUG, INFO, WARN, ERROR)

DEFINE_CONFIG_ENUM(LogLevel, {
                                 {LogLevel::DEBUG, "debug"},
                                 {LogLevel::INFO, "info"},
                                 {LogLevel::WARN, "warn"},
                                 {LogLevel::ERROR, "error"},
                             })

BOOST_LOG_ATTRIBUTE_KEYWORD(loglevel, "Severity", LogLevel)

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, log::src::severity_logger_mt<app::LogLevel>)

// TODO: define a module logger via channel logger

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

inline void initLogger(const LogConfig &config = {})
{
    // TODO: add a check that logger is not initialized yet
    // TODO: or even better, integrate this function with the global logger initialization
    // TODO: consider for file logging not to set date component (only time) - you an add a preamble handler that will
    // add date at the beginning of the file
    auto &fmt =
        log::expr::stream << log::expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f ")
                          << std::setfill(' ') << std::setw(6) << std::left << app::loglevel << log::expr::smessage;

    if (config.file)
    {
        if (config.file->rotationSize)
            log::add_file_log(log::keywords::file_name = config.file->name,
                              log::keywords::rotation_size = config.file->rotationSize.value_or(1024 * 1024),
                              log::keywords::format = std::move(fmt));
        else
            log::add_file_log(log::keywords::file_name = config.file->name, log::keywords::format = std::move(fmt));
    }
    else
    {
        log::add_console_log(std::cout, log::keywords::format = std::move(fmt));
    }
    log::core::get()->set_filter(loglevel >= config.level);
    log::add_common_attributes();
}

} // namespace app

#ifndef __FILE_NAME__
#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOG(LOG_LEVEL, ...) LOG_FORWARDER(__VA_ARGS__, LOG_TO_SELECTED, LOG_TO_GLOBAL)(LOG_LEVEL, __VA_ARGS__)
#define LOG_TO_GLOBAL(LOG_LEVEL, LOG_EXPRESSION)                                                                       \
    BOOST_LOG_SEV(app::logger::get(), LOG_LEVEL) << __FILE_NAME__ << ':' << __LINE__ << ' ' << LOG_EXPRESSION
#define LOG_TO_SELECTED(LOG_LEVEL, LOGGER, LOG_EXPRESSION)                                                             \
    BOOST_LOG_SEV((LOGGER), LOG_LEVEL) << __FILE_NAME__ << ':' << __LINE__ << ' ' << LOG_EXPRESSION
#define LOG_FORWARDER(_1, _2, NAME, ...) NAME

#define LOG_INFO(...) LOG(app::LogLevel::INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(app::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_WARN(...) LOG(app::LogLevel::WARN, __VA_ARGS__)
#define LOG_ERROR(...) LOG(app::LogLevel::ERROR, __VA_ARGS__)
