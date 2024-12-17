# Preparing dev env

## Install GCC on Ubuntu
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

## Install dependencies
```
$ sudo apt-get install catch2
```

## Building & Running
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