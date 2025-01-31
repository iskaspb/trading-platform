#include <app/init.h>
#include <io/WSS.h>

struct WebSocketClient
{
    explicit WebSocketClient(net::any_io_executor ex) : ex_(ex), timer_(ex)
    {
    }

    void operator()(WSS &ws, const OnConnected &)
    {
        LOG_INFO("WS connected to " << ws.host() << ':' << ws.port());
        ws.send("Hello, world!");
    }

    void operator()(WSS &ws, const OnDisconnected &)
    {
        LOG_INFO("WS disconnected from " << ws.host() << ':' << ws.port());
    }

    void operator()(WSS &ws, const WSData &data)
    {
        LOG_INFO("Data: " << data.data);
        timer_.expires_after(boost::asio::chrono::seconds(5));
        timer_.async_wait([&](const boost::system::error_code &ec) {
            if (ec)
                return;
            LOG_INFO("Disconnecting...");
            ws.shutdown();
        });
    }

  private:
    net::any_io_executor ex_;
    boost::asio::steady_timer timer_;
};

int main()
{
    app::init(app::LogConfig{.level = app::LogLevel::DEBUG});
    net::io_context ioc;

    WebSocketClient client(net::make_strand(ioc));
    WSS ws(std::ref(client), net::make_strand(ioc), URL("wss://echo.websocket.org"));

    ws.connect();

    ioc.run();

    return 0;
}