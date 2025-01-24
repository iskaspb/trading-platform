#pragma once

#include <util/url.h>

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class HTTPSClient
{
  public:
    HTTPSClient(net::any_io_executor ex, const URL &url)
    {
        ctx_.set_verify_mode(ssl::verify_none);
        session_ = std::make_shared<Session>(ex, ctx_, url);
    }

  private:
    struct Session : std::enable_shared_from_this<Session>
    {
        Session(net::any_io_executor ex, ssl::context &ctx, const URL &url) : stream_(ex, ctx)
        {
            tcp::resolver resolver(ex);
            resolvedURL_ = resolver.resolve(url.host(), url.port());
        }

      private:
        tcp::resolver::results_type resolvedURL_;
        beast::ssl_stream<beast::tcp_stream> stream_;
    };

    ssl::context ctx_{ssl::context::tlsv12_client};
    std::shared_ptr<Session> session_;
};