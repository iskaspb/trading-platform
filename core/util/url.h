#pragma once

#include <boost/url/url.hpp>
#include <stdexcept>

// URL is a wrapper around boost::urls::url that enforces the use of HTTPS or WSS schemes.
struct URL : boost::urls::url
{
    URL(const std::string &url) : boost::urls::url(url)
    {
        if (scheme().empty())
            throw std::logic_error("No scheme (wss or https) in URL: " + url);
        if (!(scheme() == "https" || scheme() == "wss"))
            throw std::logic_error("Unsupported scheme: " + std::string(scheme()));
        if (port().empty())
            set_port("443");

        if (path().empty())
            set_path("/");

        target_ = path();
        if (!query().empty())
            target_ += "?" + query();
        if (!fragment().empty())
            target_ += "#" + fragment();
    }
    const std::string &target() const
    {
        return target_;
    }

  private:
    std::string target_;
};