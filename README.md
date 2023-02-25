![GitHub Workflow Status (branch)](https://img.shields.io/github/workflow/status/ZenSepiol/Dockerized-Dear-ImGui/Docker%20Image%20CI/main)

# Dockerized-Dear-ImGui
This repository is designed for an easy-to-use and portable way to use Dear ImGui. It contains [Dear ImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot).
The application is compiled and run inside a docker container. During the execution of the application, the X-server of the host system is connected and the output display is forwarded to the host system.

**The only requirement to run the sample project is docker and a Debian based Linux environment (e.g. Ubuntu) that runs X-server.**

If you have problems getting the error message `Glfw Error 65544: X11: Failed to open display :0` try running `xhost +` first in your console.

## Getting started
These are the commands required to build and run the sample application:
```
# Install docker
sudo snap install docker

# Prepare the image
sudo docker compose build

# Setup the build directory
sudo docker compose run console meson setup builddir /code --native-file=native.build

# Compile the application
sudo docker compose run console ninja -C builddir

# Run the application
sudo docker compose run gui builddir/app

# Run tests
sudo docker compose run console ninja -C builddir test
sudo docker compose run console meson test -C builddir -v `test_name`

# Useful aliases
alias setup="sudo docker compose run console meson setup builddir /code --native-file=native.build"
alias console="sudo docker compose run console"
alias build="sudo docker compose run console ninja -C builddir"
alias run="sudo docker compose run gui builddir/app"
alias all_tests="sudo docker compose run console ninja -C builddir test"
alias test="sudo docker compose run console meson test -C builddir -v"
```

The sample application is stored in [app.hpp](src/app.hpp) and can be extended.
For a longer term development it is recommend to create an own application.

## Technical Details
This version of the App framework uses [GLFW](https://www.glfw.org/) and [OpenGL](https://www.opengl.org/) as backend.
As a testing framework [Catch2](https://github.com/catchorg/Catch2) is included.
As buildsystem [meson](https://mesonbuild.com) is used.

### Docker Usage
Docker compose is used to setup two different services using the same docker image. The description of both services can be found in [docker-compose](docker-compose.yml). One service called `console` is used for any interaction where no gui is required. The other service called `gui` is used for commands that require gui output. `gui` is connecting to the X-server of the host system and forwarding the output to the display of the host system. E.g. the application is running inside the docker container, but the output is displayed for any normal appliciation.

The dockerfile provided creates an image including all necessary dependencies. If additional dependencies are introduced, it is necessary to adapt the [dockerfile](Dockerfile) (docker image) accordingly.

### Dear ImGui Usage and Code Structure
The framework itself can be found [here](src/app_base/app_base.hpp). It includes the necessary backend calls to GLFW as well as OpenGL. The color schemes and the window handler are also part of the framework. They can be overriden or adapted as needed.

An example of a app based on the framework can be found [here](src/app.hpp). In order to develop a custom app, usually it is only required to adapt `app.hpp`. Additional source files that need compilation can be added in the [meson application description](meson.build).

Since it is often useful to have additional classes and tests for them, the testing framework CATCH2 is already included. An example of how to create an module and write tests is available [here](src/sample_module/). It is important to add the subdirectory to the [meson.build](meson.build) file in the root directory.


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
