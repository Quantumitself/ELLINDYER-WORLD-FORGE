# Third-Party Libraries

Vendored, header-only third-party libraries used by Ellindyer World Forge.

## Contents

    third_party/
    ├── imgui/
    ├── json/
    ├── spdlog/
    ├── stb/
    └── catch2/

## Policy

- Only header-only libraries are vendored.
- Each library keeps its original license file.
- Each library is exposed as a CMake INTERFACE target from `third_party/CMakeLists.txt`.
- No package manager is used.
- The C++ standard library and the Windows SDK remain the primary facilities.
