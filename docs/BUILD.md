# Build Instructions

## Requirements

- Windows 10 or later
- CMake 3.20 or newer
- A native C++ compiler (MSVC 2019+, clang-cl, or MinGW-w64)
- Git

## Vendored Third-Party Libraries

Before configuring the project, populate the following directories with the
corresponding header-only libraries:

- `third_party/imgui/` — Dear ImGui (with Win32 and DX11 backends)
- `third_party/json/single_include/nlohmann/json.hpp`
- `third_party/spdlog/include/spdlog/`
- `third_party/stb/stb_image.h`
- `third_party/stb/stb_image_write.h`
- `third_party/catch2/single_include/catch2/catch.hpp`

See each library's `README.md` under `third_party/` for exact file lists.

## Configure

    cmake --preset default

## Build

    cmake --build --preset default

## Run

The built executable is placed in `build/<preset>/bin/`.

## Clean

Delete the `build/` directory.
