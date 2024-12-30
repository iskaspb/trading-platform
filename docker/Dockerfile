FROM ubuntu:24.04

EXPOSE 22
ENV DEBIAN_FRONTEND=noninteractive

COPY ["./Docker_install.sh", "/tmp/Docker_install.sh"]

RUN ["/bin/bash", "/tmp/Docker_install.sh"]

CMD ["/usr/sbin/sshd","-D"]
