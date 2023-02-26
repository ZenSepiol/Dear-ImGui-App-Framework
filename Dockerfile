FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y bash libglfw3 libglfw3-dev xorg-dev libxkbcommon-dev meson clang git cmake

RUN git clone --depth 1 --branch v1.88 https://github.com/ocornut/imgui.git imgui
RUN git clone --depth 1 --branch v0.14 https://github.com/epezent/implot.git implot