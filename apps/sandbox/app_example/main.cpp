#include <app/configLocator.h>

#include <iostream>

int main(int argc, char *argv[])
{
    //...locate configuration file
    const std::string configPath = app::locateConfig(argc, argv);
    std::cout << "Found config: " << configPath << std::endl;

    //...load config
    //...initialize logger

    return 0;
}
