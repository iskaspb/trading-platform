#include <app/init.h>
#include <io/HTTPS.h>

struct AppConfig
{
    app::LogConfig log;
    nlohmann::json request;
};
DEFINE_CONFIG(AppConfig, log, request);

int main(int argc, char *argv[])
{
    const auto config = app::init<AppConfig>(argc, argv);
    const URL url(config.request);
    LOG_INFO("Request: " << url);
    net::io_context ioc;
    HTTPS client(net::make_strand(ioc), url);

    auto response = client.send(HTTPS::Get(url.target()));
    LOG_INFO("Response: code:" << response.code << "\n" << response.body);
}
