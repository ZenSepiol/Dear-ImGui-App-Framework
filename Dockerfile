FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y bash
RUN apt-get install -y libglfw3
RUN apt-get install -y libglfw3-dev
RUN apt-get install -y xorg-dev
RUN apt-get install -y libxkbcommon-dev
RUN apt-get install -y meson
RUN apt-get install -y clang
RUN apt-get install -y git
RUN apt-get install -y libssl-dev

RUN apt-get install --yes --no-install-recommends wget build-essential libcurl4 && \
    wget https://curl.se/download/curl-7.81.0.tar.gz && \
    tar -xvf curl-7.81.0.tar.gz && cd curl-7.81.0 && \
    ./configure --with-openssl && make && make install

RUN git clone --depth 1 --branch v1.88 https://github.com/ocornut/imgui.git imgui
RUN git clone --depth 1 --branch v0.14 https://github.com/epezent/implot.git implot