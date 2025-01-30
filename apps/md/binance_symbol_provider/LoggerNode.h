#pragma once
#include <app/logging.h>

template <typename Traits> struct LoggerNode
{
    struct ContextMixin
    {
        ContextMixin() : logger_(&app::logger::get())
        {
        }

        app::logger::logger_type &log()
        {
            return *logger_;
        }

      private:
        app::logger::logger_type *logger_;
    };
};
