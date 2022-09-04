# Build and run:
#   docker build . -t itba-so-tp1-grupo11:1.0 -f Dockerfile
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:2020:22 --name so-tp1-grupo11 itba-so-tp1-grupo11:1.0
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
#
# stop:
#   docker stop so-tp1-grupo11
#
# ssh credentials (test user):
#   user@password

# Based on CLion documentation
# https://blog.jetbrains.com/clion/2020/01/using-docker-with-clion/
# https://github.com/JetBrains/clion-remote/blob/master/Dockerfile.remote-cpp-env

FROM agodio/itba-so:1.0
RUN mkdir /itba
VOLUME /itba
WORKDIR /itba

# COLORS !!
# gcc
ENV GCC_COLORS="error=01;31:warning=01;35:note=01;36:range1=32:range2=34:locus=01:\
quote=01:path=01;36:fixit-insert=32:fixit-delete=31:\
diff-filename=01:diff-hunk=32:diff-delete=31:diff-insert=32:\
type-diff=01;32"

# ls
RUN echo 'alias ls="ls --color=auto"' >> /etc/bash.bashrc

RUN DEBIAN_FRONTEND="noninteractive" \
    apt install --force-yes -y \
    apt-transport-https \
    && apt update \
    && apt install --force-yes -y tzdata \
        apt-transport-https \
        ca-certificates \
        libcurl3-gnutls

RUN apt update \
    && apt install -y cmake \
        build-essential \
        gcc \
        g++ \
        gdb \
        clang \
        make \
        ninja-build \
        autoconf \
        automake \
        locales-all \
        ssh \
        rsync \
        tar

# Upgrade GDB. Clion requires GDB >= 7.8
RUN echo "deb http://ftp.de.debian.org/debian stretch main" | \
    tee /etc/apt/sources.list.d/stretch.list

RUN apt update \
    && apt install --only-upgrade --force-yes -y gdb


RUN yes password | passwd

# RUN ( echo 'LogLevel DEBUG2'; \
#     echo 'PermitRootLogin yes'; \
#     echo 'PasswordAuthentication yes'; \
#     echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
#   ) > /etc/ssh/sshd_config_clion \
#   && mkdir /run/sshd

# CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_clion"]
