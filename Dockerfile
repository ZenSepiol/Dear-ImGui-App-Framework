FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y bash
RUN apt-get install -y libglfw3
RUN apt-get install -y libglfw3-dev
RUN apt-get install -y xorg-dev
RUN apt-get install -y libxkbcommon-dev
RUN apt-get install -y meson
RUN apt-get install -y clang
RUN apt-get install -y git

RUN git clone https://github.com/ocornut/imgui.git imgui
RUN git clone https://github.com/epezent/implot implot