#pragma once

#include <io/Events.h>
#include <util/dgate.h>
#include <util/url.h>

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

struct WSData
{
    uint64_t ts{};
    std::string data;
};

class WSS;
namespace impl
{
template <typename... Events> struct WSEventsDGate
{
    using Type = core::DGate<std::function<void(WSS &ws, Events)>...>;
};
}; // namespace impl

class WSS
{
  public:
    using DGate = impl::WSEventsDGate<WSData, OnConnected, OnDisconnected>::Type;

    WSS(const DGate &dgate, net::any_io_executor ex, const URL &url)
        : WSS(dgate, ex, url.host(), url.port(), url.target())
    {
    }
    WSS(const DGate &dgate, net::any_io_executor ex, std::string_view host, std::string_view port,
        std::string_view target)
        : dgate_(dgate), ex_(ex), host_(host), port_(port), target_(target)
    {
        ctx_.set_verify_mode(ssl::verify_none);
    }

    ~WSS()
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

    bool isConnected() const
    {
        return session_.get(); // TODO: use status flag
    }

    void send(const std::string &text)
    {
        if (!isConnected())
            throw std::runtime_error(
                "WebSocket is not connected"); // TODO: use specific exception; also specify which websocket
        session_->send(text);
    }

    const std::string &host() const
    {
        return host_;
    }
    const std::string &port() const
    {
        return port_;
    }

  private:
    struct Session : std::enable_shared_from_this<Session>
    {
        static constexpr char agentName[] = "Hydrum/1.0";

        Session(WSS *parent) : parent_(parent), ws_(parent->ex_, parent->ctx_)
        {
            tcp::resolver resolver(parent->ex_);
            resolvedURL_ = resolver.resolve(parent_->host_, parent_->port_);
        }

        void connect()
        {
            beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
            beast::get_lowest_layer(ws_).async_connect(
                resolvedURL_, beast::bind_front_handler(&Session::on_connect, shared_from_this()));
        }

        void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
        {
            if (ec)
            {
                LOG_ERROR("connect: " << ec.message());
                return;
            }

            beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
            if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), parent_->host_.c_str()))
            {
                ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
                LOG_ERROR("connect: " << ec.message());
                return;
            }

            // Update the host_ string. This will provide the value of the
            // Host HTTP header during the WebSocket handshake.
            // See https://tools.ietf.org/html/rfc7230#section-5.4
            host_ = parent_->host_ + ':' + std::to_string(ep.port());

            ws_.next_layer().async_handshake(ssl::stream_base::client,
                                             beast::bind_front_handler(&Session::on_ssl_handshake, shared_from_this()));
        }

        void on_ssl_handshake(beast::error_code ec)
        {
            if (ec)
            {
                LOG_ERROR("ssl_handshake: " << ec.message());
                return;
            }

            // Turn off the timeout on the tcp_stream, because
            // the websocket stream has its own timeout system.
            beast::get_lowest_layer(ws_).expires_never();

            // Set suggested timeout settings for the websocket
            ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

            // Set a decorator to change the User-Agent of the handshake
            ws_.set_option(websocket::stream_base::decorator(
                [](websocket::request_type &req) { req.set(http::field::user_agent, std::string(agentName)); }));

            // Perform the websocket handshake
            ws_.async_handshake(host_, parent_->target_,
                                beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
        }

        void on_handshake(beast::error_code ec)
        {
            if (ec)
            {
                LOG_ERROR("handshake: " << ec.message());
                return;
            }

            LOG_DEBUG("WebSocket connected to " << host_);
            parent_->dgate_(*parent_, OnConnected{});

            ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
        }

        void on_read(beast::error_code ec, std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if (ec)
            {
                // LOG_ERROR("read: " << ec.message()); //TODO: find a way to cancel the read operation during shutdown
                // so we do not get error code here
                return;
            }

            parent_->dgate_(*parent_, WSData{.data = beast::buffers_to_string(buffer_.data())});
            buffer_.consume(buffer_.size());

            ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
        }

        void send(const std::string &text)
        {
            // TODO: use status flag to check if connected
            // TODO: consider if WS should use async_write (in this case you need to maintain the write status and
            // queue)
            ws_.write(net::buffer(text));
        }

        void shutdown()
        {
            // TODO: cancel all async operations before closing the connection for clean error handling
            ws_.async_close(websocket::close_code::normal,
                            beast::bind_front_handler(&Session::on_close, shared_from_this()));
        }

        void on_close(beast::error_code ec)
        {
            if (ec)
            {
                LOG_ERROR("close: " << ec.message());
                return;
            }
            parent_->dgate_(*parent_, OnDisconnected{});
        }

        WSS *const parent_;
        websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
        tcp::resolver::results_type resolvedURL_;
        std::string host_;
        beast::flat_buffer buffer_;
    };

  private:
    const DGate dgate_;
    net::any_io_executor ex_;
    ssl::context ctx_{ssl::context::tlsv12_client};
    std::shared_ptr<Session> session_;
    std::string host_, port_, target_;
};
