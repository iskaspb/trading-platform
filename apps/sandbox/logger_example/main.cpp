#include <app/logging.h>

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#include <iostream>

/*
See notes on log level :
https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/log/detailed/expressions.html#log.detailed.expressions.attr.tags

Name scope formatter :
https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/log/detailed/expressions.html#log.detailed.expressions.formatters.named_scope
https://www.boost.org/doc/libs/1_82_0/libs/log/doc/html/log/detailed/attributes.html#log.detailed.attributes.named_scope
*/

int main()
{
    // BOOST_LOG_SEV(app::logger::get(), app::logging::trivial::info) << "Logger example output";
    LOG_INFO("Logger example output with data : " << 10);
    LOG_INFO(app::logger::get(), "Explicit logger selection with data : " << 30);
}
