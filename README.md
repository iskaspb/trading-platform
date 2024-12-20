## Prepare dev env
You may choose to build and run the platform and associated apps natively on your machine or you can use docker container.
Docker path is less cumbersome in terms of dependency management, however it provides less integration with your host dev env.
<details>
<summary><b>Native setup on dev box</b></summary>

### Install GCC on Ubuntu
Refreshing the apt database:

```
$ sudo apt update
$ sudo apt-get upgrade -y
$ sudo apt-get dist-upgrade -y
```
Installing dev tools:
```
$ sudo apt install build-essential software-properties-common manpages-dev -y
```
Adding the GCC apt repo:
```
$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
$ sudo apt-get update -y
```
Install latest (as of 2024-DEC) GCC:
```
$ sudo apt install gcc-14 g++-14 -y
```
Configure the links to default compiler:
```
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14 --slave /usr/bin/g++ g++ /usr/bin/g++-14
```
Select and check gcc version
```
$ sudo update-alternatives --config gcc
There is 1 choice for the alternative gcc (providing /usr/bin/gcc).

  Selection    Path             Priority   Status
------------------------------------------------------------
* 0            /usr/bin/gcc-14   14        auto mode
  1            /usr/bin/gcc-14   14        manual mode

$ gcc --version
gcc (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0
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

</details>

<details>
<summary><b>Use pre-built docker image</b></summary>
TODO...
</details>

<details>
<summary><b>Build your own docker image</b></summary>
TODO...
</details>

### Build & Run
This will build everything and runs available unit tests:
```
$ mkdir build && cd build
build $ cmake .. -DCMAKE_BUILD_TYPE=Debug
...
build $ make all test
...
Running tests...
Test project /home/alex/src/trading-platform/build
    Start 1: basic_test
1/1 Test #1: basic_test .......................   Passed    0.00 sec
```
For more interesting examples you can try [apps and tools](apps/README.md)

<details>
<summary><b>Tips and tricks for VSCode</b></summary>

1. Install CMake tools and C/C++ Extension Pack plugins;
2. Configure project using CMake (you can see it in the status bar in the bottom) - you might need to select GCC version in the drop down menu;
3. To build project you can press "Build" in the status bar (or you can do the same but select a specific target instead of "all");
4. CTRL-SHIFT-B to build (you might need to generate a task to skip drop down menu - this is done by selecting config button when the drop down menu appears);
5. CTRL-F5 to build and run (it's useful for tests - you can select basic_test as a target to try it)
 
</details>

## Platform components
1. [Core framework](core/README.md)
2. [Applicaitons and tools](apps/README.md)
3. [Unit tests](tests/README.md)

