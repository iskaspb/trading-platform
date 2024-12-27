#include <app/logging.h>

#include <iostream>

std::string getLogData()
{
    std::cout << "You should not see this message as it's part of DEBUG which is suppressed" << std::endl;
    return "Log data";
}

int main()
{
    app::initLogger();

    auto &lg = app::logger::get();
    using enum app::LogLevel;

    LOG_DEBUG("A debug severity message: " << getLogData());
    LOG_INFO(lg, "A normal severity message with data : " << 10);
    LOG(WARN, lg, "A warning severity message");
    BOOST_LOG_SEV(lg, ERROR) << "An error severity message (standard macros, without file and line)";
}
