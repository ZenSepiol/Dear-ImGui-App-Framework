FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y bash libglfw3 libglfw3-dev xorg-dev libxkbcommon-dev meson clang git cmake libssl-dev cmake

RUN apt-get install --yes --no-install-recommends wget build-essential libcurl4 && \
    wget https://curl.se/download/curl-7.81.0.tar.gz && \
    tar -xvf curl-7.81.0.tar.gz && cd curl-7.81.0 && \
    ./configure --with-openssl && make && make install

RUN git clone --depth 1 --branch v1.88 https://github.com/ocornut/imgui.git imgui
RUN git clone --depth 1 --branch v0.14 https://github.com/epezent/implot.git implot