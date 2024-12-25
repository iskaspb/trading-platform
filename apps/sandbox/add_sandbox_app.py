#!/usr/bin/python3

import os
import argparse

os.chdir(os.path.dirname(os.path.abspath(__file__)))


def createCmake(appName):
    content = f"""file(GLOB SRC *.cpp)
app(
    {appName} EXCLUDE_FROM_ALL SRC ${{SRC}}
    #COMPILER_OPTIONS ...
    #LINK_OPTIONS ${{Boost_LIBRARIES}} ...
)
"""
    with open(f"{appName}/CMakeLists.txt", "w") as f:
        f.write(content)


def createMain(appName):
    content = f"""#include <iostream>

int main() {{
    std::cout << "Hello, {appName}!" << std::endl;
    return 0;
}}
"""
    with open(f"{appName}/main.cpp", "w") as f:
        f.write(content)

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
    touch(f"{appName}/CMakeLists.txt")
    print(f"Added sandbox app in folder {appName}")


if __name__ == "__main__":
    main()
