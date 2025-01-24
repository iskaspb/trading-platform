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

class HTTPS
{
    static constexpr int version = 11;
    static constexpr char agentName[] = "Hydrum/1.0";

  public:
    HTTPS(net::any_io_executor ex, const URL &url) : HTTPS(ex, url.host(), url.port())
    {
    }
    HTTPS(net::any_io_executor ex, std::string_view host, std::string_view port) : ex_(ex), host_(host), port_(port)
    {
        ctx_.set_verify_mode(ssl::verify_none);
    }
    ~HTTPS()
    {
        shutdown();
    }

    void connect()
    {
        session_ = std::make_shared<Session>(this);
        session_->connect();
    }

    void shutdown()
    {
        if (session_)
        {
            session_->shutdown();
            session_.reset();
        }
    }

    struct Get
    {
        Get(std::string_view target)
        {
            req_.method(http::verb::get);
            req_.target(target);
            req_.version(version);
            req_.set(http::field::user_agent, agentName);
        }

        Get &field(http::field field, std::string_view value)
        {
            req_.set(field, value);
            return *this;
        }

      private:
        friend class HTTPS;
        mutable http::request<http::string_body> req_;
    };

    struct Response
    {
        unsigned code;
        std::string body;
    };

    Response send(const Get &get)
    {
        if (!session_)
            connect();

        get.req_.set(http::field::host, host_);
        http::response<http::string_body> res;
        session_->send(get.req_, res);
        return Response{res.result_int(), res.body()};
    }

  private:
    struct Session : std::enable_shared_from_this<Session>
    {
        Session(HTTPS *parent) : parent_(parent), stream_(parent->ex_, parent->ctx_)
        {
            tcp::resolver resolver(parent->ex_);
            resolvedURL_ = resolver.resolve(parent_->host_, parent_->port_);
        }

        void connect()
        {
            beast::get_lowest_layer(stream_).connect(resolvedURL_);
            stream_.handshake(ssl::stream_base::client);
        }

        void shutdown()
        {
            beast::error_code ec;
            stream_.shutdown(ec);
            if (ec == net::error::eof || ec == boost::asio::ssl::error::stream_truncated)
                ec = {};
            if (ec)
                LOG_WARN("HTTPS[" << parent_->host_ << ':' << parent_->port_ << "] shutdown: " << ec.message());
        }

        void send(const http::request<http::string_body> &req, http::response<http::string_body> &res)
        {
            if (!beast::get_lowest_layer(stream_).socket().is_open())
                connect();

            http::write(stream_, req);
            http::read(stream_, buffer_, res);
            buffer_.consume(buffer_.size());
        }

      private:
        HTTPS *parent_;
        beast::ssl_stream<beast::tcp_stream> stream_;
        tcp::resolver::results_type resolvedURL_;
        beast::flat_buffer buffer_;
    };

    net::any_io_executor ex_;
    ssl::context ctx_{ssl::context::tlsv12_client};
    std::shared_ptr<Session> session_;
    std::string host_, port_;
};