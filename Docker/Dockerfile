
FROM ubuntu:xenial
MAINTAINER Adrien Chardon <chardond@edu.ece.fr>

# install packages

RUN apt-get update -q && \
    apt-get install -q -y --no-install-recommends \
        build-essential \
        bzip2 \
        git \
        lib32stdc++6 \
        make \
        openssh-client \
        python-dev \
        python-pip \
        python-setuptools \
        wget

RUN pip install --upgrade setuptools pip

# install compiler

WORKDIR /opt

RUN wget -q https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
RUN bunzip2 gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
RUN tar xf gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar
ENV PATH="${PATH}:/opt/gcc-arm-none-eabi-5_4-2016q3/bin"

# install mbed-cli and clone mbed-os

WORKDIR /opt

RUN pip install mbed-cli

RUN git clone https://github.com/ARMmbed/mbed-os.git
WORKDIR /opt/mbed-os
RUN pip install -r requirements.txt

# get mbed libs

RUN mkdir /opt/mbed-libs
WORKDIR /opt/mbed-libs
RUN wget -q https://developer.mbed.org/users/sam_grove/code/BufferedSerial/archive/a0d37088b405.tar.gz -O BufferedSerial.tar.gz
RUN wget -q https://developer.mbed.org/users/sam_grove/code/Buffer/archive/89564915f2a7.tar.gz
RUN wget -q https://developer.mbed.org/users/aberk/code/PID/archive/6e12a3e5af19.tar.gz
#RUN wget -q https://developer.mbed.org/users/aberk/code/QEI/archive/5c2ad81551aa.tar.gz
RUN for file in $(ls *.tar.gz); do tar xzf $file; done

# set up the build folder

RUN mkdir /tmp/build

# copy the files mounted in read only and make

WORKDIR /tmp/build
COPY build.sh ./
ENTRYPOINT ./build.sh
