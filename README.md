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
$ gcc --version

gcc (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0
...
```
