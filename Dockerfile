FROM nvidia/opengl:base-ubuntu22.04

ENV DISPLAY=:0

RUN apt-get update
RUN apt-get install -y bash libglfw3 libglfw3-dev xorg-dev libxkbcommon-dev clang git cmake libssl-dev cmake libboost-dev libsfml-dev
RUN apt-get install -y libeigen3-dev

COPY . /

RUN ./scripts/build_compile.sh