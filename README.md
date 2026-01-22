# X11 Software Renderer Engine (WIP)

A minimal, from-scratch 2D software rendering engine built for Linux using X11 and C++20. 

This project demonstrates how to create a window, handle inputs, and manually render pixels to a framebuffer without relying on modern graphics APIs like OpenGL or Vulkan.

## Status: Work in Progress
This engine is currently in active development.
- [x] Basic X11 Window Creation
- [x] Software Framebuffer (Pixel Plotting)
- [x] Input System (Keyboard)
- [x] Basic Entity/Object Architecture

## Prerequisites

To build this project, you need a Linux environment with the following dependencies:

* **C++ Compiler**: GCC or Clang (Must support C++20)
* **CMake**: Version 3.20 or higher
* **X11 Development Libraries**:
    * Ubuntu/Debian: `sudo apt install libx11-dev`
    * Arch: `sudo pacman -S libx11`
    * Fedora: `sudo dnf install libX11-devel`

## Build Instructions

1.  **Clone the repository**:
    ```bash
    git clone <your-repo-url>
    cd X11_engine
    ```

2.  **Create a build directory**:
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake**:
    ```bash
    cmake ..
    ```

4.  **Build the executable**:
    ```bash
    cmake --build .
    ```

## Usage

After building, the binary can be found in the `build/bin` directory (or wherever your CMake configures output).

```bash
./bin/X11Engine
```
