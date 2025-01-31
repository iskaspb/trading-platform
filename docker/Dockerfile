FROM ubuntu:24.04

EXPOSE 22
ENV DEBIAN_FRONTEND=noninteractive

# Set default folder for config files
ENV APP_CONFIG_DIR=/home/user/trading-platform/build/config

COPY ["./Docker_install.sh", "/tmp/Docker_install.sh"]

RUN ["/bin/bash", "/tmp/Docker_install.sh"]

CMD ["/usr/sbin/sshd","-D"]
