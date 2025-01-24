#pragma once
#include <boost/asio.hpp>

namespace net = boost::asio;

template <typename Traits> struct AsioService
{
    struct ContextMixin
    {
        net::io_context &getIO()
        {
            return ioc;
        }

      private:
        net::io_context ioc;
    };
};
