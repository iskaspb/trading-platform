# Stop at the first error, so a failed step fails the image build
set -e

# Basic system setup (build-essential brings GCC 15, the default compiler on Ubuntu 26.04)
apt-get update
apt-get install --assume-yes \
    build-essential \
    gdb \
    cmake \
    git \
    pkg-config \
    unzip \
    uuid-dev \
    openssh-server \
    sudo \
    mc

# Install dependencies
apt-get install --assume-yes \
    libboost-all-dev \
    nlohmann-json3-dev \
    rapidjson-dev \
    catch2 \
    libssl-dev \
    libfmt-dev \
    capnproto


# Create user
useradd -rm -d /home/user -s /bin/bash -G sudo user
echo 'user:password' | chpasswd

# Start ssh
service ssh start
