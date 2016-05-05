# FlickFlow

## Description

FlickFlow is an incompressible fluid simulation written OpenGL, with bindings to the Leap motion SDK for additional interactivity. [Here](https://docs.google.com/presentation/d/1Ha1kNUi-22188_0rZ7o7jt-xS9Eb8eh20hNoLjJnnEQ) is a presentation detailing a high-level overview of how it works.

![](https://cloud.githubusercontent.com/assets/4731228/15039261/6f7eba3e-1260-11e6-87ec-3ed430fe0429.gif)

![](https://cloud.githubusercontent.com/assets/4731228/15039340/2293f864-1261-11e6-8671-ae9d9b5f27eb.gif)

![](https://cloud.githubusercontent.com/assets/4731228/15039525/a11b4de4-1262-11e6-999a-e0ea80bec301.gif)

## Installation

The build process has only been tested on Mac OS X (>=10.11). It should be trivially portable to Linux, but this project was not written with Windows in mind. No Visual Studio build files were created (if you do create one, please submit a pull request!).

Your system must have CMake, Freetype, OpenGL, GLEW, GLFW, and the Leap Motion SDK installed. The best place to get CMake, Fretype, GLEW and GLFW is through [Homebrew](http://brew.sh/). Make sure that the files in the Leap motion includes folder (`Leap.h`, `LeapMath.h`, etc.) are copied to `/usr/local/includes` and that `lib/libLeap.dylib` is copied to `/usr/local/lib/`. After all the dependencies are met, simply use CMake to build the Makefile, then run `make` to build FlickFlow:

```
$ cd flickflow
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./flickflow
```

## Usage

Simply use the mouse or Leap motion to interact with the simulation. Use the numbers 1-5 to switch between different visualization modes:

1. Dye
2. Velocity
3. Pressure
4. Divergence
5. Vorticity

Further simulation constants can be set inside of `fluidDynamics.cpp`. Interesting parameters include `DISSIPATION` and `VELOCITY_DISSIPATION`, which control how long ink stays on the screen, along with `CURL` and `VISCOSITY`, which govern the vorticity forces in the fluid. `INK_SPLAT_SIZE` and `VELOCITY_SPLAT_SIZE` can also be adjusted in `main.cpp` to change the size of the ink and velocity splats created through interactions.

