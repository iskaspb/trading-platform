#include <app/configLocator.h>

#include <iostream>

int main()
{
    //...locate configuration file
    const std::string configPath = app::locateConfig();
    std::cout << "Config path: " << configPath << std::endl;

    //...load config
    //...initialize logger

    return 0;
}
