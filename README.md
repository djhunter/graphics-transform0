# graphics-project0

Simple starter project for computer graphics class.

This is a simple cross-platform skeleton project for students to use for their projects in my Computer Graphics class. It uses GLFW: http://www.glfw.org/

## Prerequisites:
  * Linux: build-essential xorg-dev cmake libvulkan-dev doxygen
  * OSX: Xcode, cmake
  * WIN: Microsoft SDK (which comes with Visual C++) and cmake

In addition, for all platforms, install the glm headers. This is a header-only library, so you just need to copy the `glm` directory into your include path (e.g., `/usr/local/include`). Use the latest release: http://glm.g-truc.net/

## Instructions for students:

  1. Clone this repository.
  2. Perform an in-tree build:
      ```
      cmake .
      make
      ```

