#include <app/logging.h> //...this header file has all logging facilities

#include <iostream>

std::string getLogData() //...this function tests if log-level suppression works as expected (default config supresses anyting lower than INFO)
{
    std::cout << "You should not see this message as it's part of DEBUG which is suppressed" << std::endl;
    return "Log data";
}

int main()
{
    app::initLogger(); //...should be called at the beginning (it's also part of "Application" helper class so you don't need to be called if you use Application to initalize the app). Accepts logger configuration (here the default config is used) and initilizes logger

    auto &lg = app::logger::get(); //..."caches" reference to global logger so we don't call singleton accessor every time
    using enum app::LogLevel;

    LOG_DEBUG("A debug severity message: " << getLogData()); //...this messagage is suppressed from output
    LOG_INFO(lg, "A normal severity message with data : " << 10); //..."typical" way of using logger
    LOG(WARN, lg, "A warning severity message"); //...you can use more general LOG macro with log-level as the first parameter
    BOOST_LOG_SEV(lg, ERROR) << "An error severity message (standard macros, without file and line)"; //...this macro is provided by Boost, it doesn't do some of the output, but still might be useful
}
