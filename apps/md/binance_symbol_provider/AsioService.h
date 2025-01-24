#pragma once
#include <app/logging.h>

#include <boost/asio.hpp>

namespace net = boost::asio;

template <typename Traits> struct AsioService
{
    struct ContextMixin
    {
        ContextMixin()
        {
            LOG_DEBUG("AsioService initialized");
        }
        net::io_context &getIO()
        {
            return ioc;
        }

      private:
        net::io_context ioc;
    };
};
