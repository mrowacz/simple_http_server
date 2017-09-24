FROM ubuntu:16.04

RUN apt-get update && apt-get -y install \
         automake \
         build-essential \
         cmake \
         git \
         cmake \
		 libcurl4-openssl-dev \
		 libsqlite3-dev \
		 libboost-all-dev
COPY . /root/source/
RUN cd /root/source && mkdir build && cd build && cmake .. && make
RUN /root/source/build/runTests
