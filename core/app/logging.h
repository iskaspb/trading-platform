#pragma once

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace app
{
namespace logging
{
using namespace boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
} // namespace logging

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, logging::src::logger_mt)

} // namespace app

#define LOG_INFO(...) LOG_INFO_IMPL(__VA_ARGS__, LOG_INFO_TWO_ARGS, LOG_INFO_ONE_ARG)(__VA_ARGS__)
#define LOG_INFO_ONE_ARG(LOG_EXPRESSION) BOOST_LOG(app::logger::get()) << LOG_EXPRESSION
#define LOG_INFO_TWO_ARGS(LOGGER, LOG_EXPRESSION) BOOST_LOG(LOGGER) << LOG_EXPRESSION
#define LOG_INFO_IMPL(_1, _2, NAME, ...) NAME
