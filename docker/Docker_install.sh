# Basic system setup
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

# Install gcc-14
add-apt-repository ppa:ubuntu-toolchain-r/test -y
apt-get update
apt-get install --assume-yes \
    build-essential software-properties-common manpages-dev \
    gcc-14 g++-14
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14 --slave /usr/bin/g++ g++ /usr/bin/g++-14

# Install dependencies
apt-get install --assume-yes \
    libboost-all-dev \
    nlohmann-json3-dev \
    rapidjson-dev \
    catch2 \
    libssl-dev \
    libfmt-dev

# Create user
useradd -rm -d /home/user -s /bin/bash -G sudo user
echo 'user:password' | chpasswd

# Start ssh
service ssh start
