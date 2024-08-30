FROM nvidia/opengl:base-ubuntu22.04

COPY . /

ENV DISPLAY=:0

RUN apt-get update
RUN apt-get install -y bash libglfw3 libglfw3-dev xorg-dev libxkbcommon-dev clang git cmake libssl-dev cmake libboost-dev libsfml-dev
RUN apt-get install -y libeigen3-dev

RUN mkdir build

WORKDIR /build

RUN cmake ..

RUN make