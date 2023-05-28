<a href="https://www.buymeacoffee.com/zensepiol"><img src="https://www.buymeacoffee.com/assets/img/custom_images/yellow_img.png" height="21px"></a> [![Module tests](https://github.com/ZenSepiol/Dear-ImGui-App-Framework/actions/workflows/module-tests.yml/badge.svg)](https://github.com/ZenSepiol/Dear-ImGui-App-Framework/actions/workflows/module-tests.yml) [![Docker Image CI](https://github.com/ZenSepiol/Dear-ImGui-App-Framework/actions/workflows/docker-image.yml/badge.svg)](https://github.com/ZenSepiol/Dear-ImGui-App-Framework/actions/workflows/docker-image.yml)

# Dear-ImGui-App-Framework
This repository is used to create simple applications based on Dear ImGui. It tries to keep dependencies to a minimum, and offer a fast and easy solution to build C++ apps with GUI.

## Technical Details
This version of the App framework uses [GLFW](https://www.glfw.org/) and [OpenGL](https://www.opengl.org/) as backend.
As a testing framework [Catch2](https://github.com/catchorg/Catch2) is included.
As buildsystem [meson](https://mesonbuild.com) is used.

## Dear ImGui Usage and Code Structure
In [lib](src/lib/) is the library code and in [app](src/app/) are examples on the usage.
The framework itself can be found [here](src/lib/app_base/app_base.hpp). It includes the necessary backend calls to GLFW as well as OpenGL. The color schemes and the window handler are also part of the framework. They can be overriden or adapted as needed.

An example of a app based on the framework can be found [here](src/app/sample_app/app.hpp). In order to develop a custom app, usually it is only required to adapt `app.hpp`. Additional source files that need compilation can be added in the [meson application description](meson.build).

Since it is often useful to have additional classes and tests for them, the testing framework CATCH2 is already included. An example of how to create an module and write tests is available [here](src/app/sample_app/sample_module/). It is important to add the subdirectory to the [meson.build](meson.build) file in the root directory.

## Setup
### Running on a standard linux (e.g. Ubuntu)
The following packages are needed to run build and run the application:
```
apt-get install -y bash libglfw3 libglfw3-dev xorg-dev libxkbcommon-dev meson clang git cmake libssl-dev cmake

apt-get install --yes --no-install-recommends wget build-essential libcurl4 && \
    wget https://curl.se/download/curl-7.81.0.tar.gz && \
    tar -xvf curl-7.81.0.tar.gz && cd curl-7.81.0 && \
    ./configure --with-openssl && make && make install
```
Additionally, it is necessary to clone Dear ImGui and ImPlot:
```
    git clone --depth 1 --branch v1.88 https://github.com/ocornut/imgui.git /imgui
    git clone --depth 1 --branch v0.14 https://github.com/epezent/implot.git /implot
```

Useful commands for building and running the application.

```
# Setup the build directory
meson setup builddir --native-file=native.build

# Compile the application
ninja -C builddir src/app/sample_app/sample_app

# Run the application
./builddir/src/app/sample_app/sample_app

# Run all tests
ninja -C builddir test
meson test -C builddir

# Running a specific test
ninja -C builddir src/app/sample_app/sample_module/sample_module_test && ./builddir/src/app/sample_app/sample_module/sample_module_test
meson test -C builddir -v sample_module_test
```


### Running in Docker
This repository is designed for an easy-to-use and portable way to use Dear ImGui. It contains [Dear ImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot).
The application is compiled and run inside a docker container. During the execution of the application, the X-server of the host system is connected and the output display is forwarded to the host system.

**The only requirement to run the sample project is docker and a Debian based Linux environment (e.g. Ubuntu) that runs X-server.**

#### Install docker
To install docker you can find more information [here](https://docs.docker.com/engine/install/ubuntu/)

Using the install script allows easy installation of docker.
```
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh ./get-docker.sh --dry-run
```
Afterwards
```
# Prepare the image
docker compose build

# Setup the build directory
docker compose run console meson setup builddir /code --native-file=native.build

# Compile the application
docker compose run console ninja -C builddir

# Run the application
docker compose run gui builddir/src/app/sample_app/sample_app

# Run tests
docker compose run console ninja -C builddir test
docker compose run console meson test -C builddir -v `test_name`

# Useful aliases
alias setup="docker compose run console meson setup builddir /code --native-file=native.build"
alias console="docker compose run console"
alias build="docker compose run console ninja -C builddir"
alias run="docker compose run gui"
alias all_tests="docker compose run console ninja -C builddir test"
alias test="docker compose run console meson test -C builddir -v"
```

The sample application is stored in [app.hpp](src/app.hpp) and can be extended.
For a longer term development it is recommend to create an own application.

#### Docker Usage
Docker compose is used to setup two different services using the same docker image. The description of both services can be found in [docker-compose](docker-compose.yml). One service called `console` is used for any interaction where no gui is required. The other service called `gui` is used for commands that require gui output. `gui` is connecting to the X-server of the host system and forwarding the output to the display of the host system. E.g. the application is running inside the docker container, but the output is displayed for any normal appliciation.

The dockerfile provided creates an image including all necessary dependencies. If additional dependencies are introduced, it is necessary to adapt the [dockerfile](Dockerfile) (docker image) accordingly.

#### Docker Troubleshooting
If you have problems getting the error message `Glfw Error 65544: X11: Failed to open display :0` try running `xhost +` first in your console.

If you do not want to share the X server to the host, it is possible to disable the MIT-SHM support for the X server by creating a file
`/etc/X11/xorg.conf.d/disable-MIT-SHM.conf` with the content.
```
Section "Extensions"
    Option "MIT-SHM" "Disable"
EndSection
```
Afterwards it is possible to remove `network_mode: host` from the `docker-compose.yml`.

It's also possible to run without using `xhost +` by adding this snippet to the dockerfile:
```
RUN export uid=1000 gid=1000 && \
    mkdir -p /home/developer && \
    echo "developer:x:${uid}:${gid}:Developer,,,:/home/developer:/bin/bash" >> /etc/passwd && \
    echo "developer:x:${uid}:" >> /etc/group && \
    mkdir -p /etc/sudoers.d && \
    echo "developer ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/developer && \
    chmod 0440 /etc/sudoers.d/developer && \
    chown ${uid}:${gid} -R /home/developer

USER developer
ENV HOME /home/developer
```
