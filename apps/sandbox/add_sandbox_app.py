#!/usr/bin/python3

import os
import argparse
import json

os.chdir(os.path.dirname(os.path.abspath(__file__)))


def createCmake(appName):
    content = f"""file(GLOB SRC *.cpp)
app(
    {appName} EXCLUDE_FROM_ALL SRC ${{SRC}}
    #COMPILER_OPTIONS ...
    LINK_OPTIONS ${{Boost_LIBRARIES}} nlohmann_json::nlohmann_json #...
)
"""
    with open(f"{appName}/CMakeLists.txt", "w") as f:
        f.write(content)


def createMain(appName):
    content = f"""#include <app/init.h>

struct AppConfig
{{
    app::LogConfig log;
    nlohmann::json other;
}};
DEFINE_CONFIG(AppConfig, log, other);

int main(int argc, char *argv[])
{{
    const auto config = app::init<AppConfig>(argc, argv);
    //...add your code here
    LOG_INFO("Hello, {appName}!");
}}
"""
    with open(f"{appName}/main.cpp", "w") as f:
        f.write(content)

def createConfig(appName):
    os.makedirs(f"{appName}/config", exist_ok=True)
    config = {"log" : {"level" : "debug"}, "other" : {"param1" : "value1", "param2" : "value2"}}
    with open(f"{appName}/config/{appName}.json", "w") as f:
        f.write(json.dumps(config, indent=4))


def touch(filename):
    with open(filename, 'a'):
        os.utime(filename, None)

def main():
    parser = argparse.ArgumentParser(description="Create a new sandbox app")
    parser.add_argument("appName", type=str, help="Name of the new app")
    args = parser.parse_args()

    appName = args.appName

    if " " in appName:
        print("Error: 'appName' should not contain spaces.")
        return
    if "test" == appName:
        print("Error: 'test' is reserved name, please choose another name.")
        return

    if os.path.exists(appName):
        print(f"Error: Folder {appName} already exists.")
        return

    os.mkdir(appName)
    createCmake(appName)
    createMain(appName)
    createConfig(appName)
    touch(f"CMakeLists.txt")
    print(f"Added sandbox app in folder {appName}")


if __name__ == "__main__":
    main()
