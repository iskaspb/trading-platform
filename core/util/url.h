#pragma once

#include <boost/url/url.hpp>
#include <stdexcept>

// URL is a wrapper around boost::urls::url that enforces the use of HTTPS or WSS schemes.
struct URL : boost::urls::url
{
    explicit URL(const std::string &url) : boost::urls::url(url)
    {
        if (scheme() != "https" && scheme() != "wss")
            throw std::logic_error("Unsupported scheme: " + std::string(scheme()));
        if (port().empty())
            set_port("443");
    }
};