# FlickFlow

## Description

FlickFlow is an incompressible fluid simulation written OpenGL, with bindings to the Leap motion SDK for additional interactivity. [Here](https://docs.google.com/presentation/d/1Ha1kNUi-22188_0rZ7o7jt-xS9Eb8eh20hNoLjJnnEQ) is a presentation detailing a high-level overview of how it works.

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

