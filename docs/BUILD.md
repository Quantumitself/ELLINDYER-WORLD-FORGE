# Build Instructions

## Requirements

- Windows 10 or later
- CMake 3.20 or newer
- A native C++ compiler (MSVC 2019+, clang-cl, or MinGW-w64)
- Git

## Configure

    cmake --preset default

## Build

    cmake --build --preset default

## Run

The built executable is placed in `build/<preset>/bin/`.

## Clean

Delete the `build/` directory.
