## About
A foundation repo for building an HFT crypto trading system in C++23. It contains:
- a CMake build system and the setup for its third-party dependencies (native or Docker);
- a core framework: configuration, logging, component assembly and async HTTPS/WebSocket I/O, with unit tests;
- illustrative components, such as [binance_md_capture](apps/md/binance_md_capture), which streams Binance trades over WebSocket.

## Getting the code
```
git clone git@github.com:iskaspb/trading-platform.git && cd trading-platform
git submodule update --init --recursive
```

## Prepare dev env
You may choose to build and run the platform and associated apps natively on your machine or you can use docker container.
Docker path is less cumbersome in terms of dependency management, however it provides less integration with your host dev env.
<details>
<summary><b>Native setup on dev box</b></summary>

### Install GCC on Ubuntu 26.04
GCC 15 is the default compiler on Ubuntu 26.04, so installing dev tools is enough:
```
$ sudo apt update
$ sudo apt install build-essential cmake -y
```
Check gcc version
```
$ gcc --version
gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0
...
```

### Install dependencies
Collection of [Boost libraries](https://www.boost.org/):
```
$ sudo apt-get install libboost-all-dev -y
```

[JSON parser for Modern C++](https://github.com/nlohmann/json):
```
$ sudo apt-get install nlohmann-json3-dev -y
```

[Low abstraction JSON parser](https://rapidjson.org/):
```
$ sudo apt-get install rapidjson-dev -y
```

[Catch2 unit testing framework](https://github.com/catchorg/Catch2):
```
$ sudo apt-get install catch2 -y
```

[OpenSSL](https://www.openssl.org/) for HTTPS and WSS:
```
$ sudo apt-get install libssl-dev -y
```

[{fmt} formatting library](https://fmt.dev/):
```
$ sudo apt-get install libfmt-dev -y
```

[Cap'n Proto serialization](https://capnproto.org/):
```
$ sudo apt-get install capnproto -y
```

</details>

<details>
<summary><b>Use pre-built docker image</b></summary>
TODO...
</details>

<details>
<summary><b>Build your own docker image</b></summary>

### Build and run docker image locally
```
$ docker build -t trading-platform ./docker
...
$ docker run -it --name trading-platform -dp 2223:22 \
    --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
    -v /Users/alex/work/trading-platform:/home/user/trading-platform trading-platform
$ ssh user@localhost -p 2223
  -> password
```

</details>

### Build & Run
This will build everything and run available unit tests:
```
trading-platform$ mkdir build && cd build
trading-platform/build$ cmake .. -DCMAKE_BUILD_TYPE=Debug
...
trading-platform/build$ make all test
...
Running tests...
Test project /home/alex/src/trading-platform/build
    Start 1: core_test
1/1 Test #1: core_test ........................   Passed    0.00 sec
```
For more interesting examples you can try [apps and tools](apps/README.md)

<details>
<summary><b>Tips and tricks for VSCode</b></summary>

1. Install CMake tools and C/C++ Extension Pack plugins;
2. Configure project using CMake (you can see it in the status bar in the bottom) - you might need to select GCC version in the drop down menu;
3. To build project you can press "Build" in the status bar (or you can do the same but select a specific target instead of "all");
4. CTRL-SHIFT-B to build (you might need to generate a task to skip drop down menu - this is done by selecting config button when the drop down menu appears);
5. CTRL-F5 to build and run (it's useful for tests - you can select core_test as a target to try it)
 
</details>

## Platform components
1. [Core framework](core/README.md)
2. [Applicaitons and tools](apps/README.md)
3. [Unit tests](tests/README.md)
