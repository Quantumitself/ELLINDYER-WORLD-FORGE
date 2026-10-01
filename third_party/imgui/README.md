# Dear ImGui

Immediate-mode UI framework used by Ellindyer World Forge.

## Required Files

Place the following files from the Dear ImGui `docking` branch (or a pinned
release) into this directory:

    imgui/
    ├── imgui.cpp
    ├── imgui.h
    ├── imgui_draw.cpp
    ├── imgui_internal.h
    ├── imgui_tables.cpp
    ├── imgui_widgets.cpp
    ├── imconfig.h
    ├── imstb_rectpack.h
    ├── imstb_textedit.h
    ├── imstb_truetype.h
    ├── LICENSE.txt
    └── backends/
        ├── imgui_impl_win32.cpp
        ├── imgui_impl_win32.h
        ├── imgui_impl_dx11.cpp
        └── imgui_impl_dx11.h

## Build Notes

Dear ImGui is compiled as part of the application target via
`third_party/CMakeLists.txt`. The following translation units are added to
the application build:

- `third_party/imgui/imgui.cpp`
- `third_party/imgui/imgui_draw.cpp`
- `third_party/imgui/imgui_tables.cpp`
- `third_party/imgui/imgui_widgets.cpp`
- `third_party/imgui/backends/imgui_impl_win32.cpp`
- `third_party/imgui/backends/imgui_impl_dx11.cpp`

## License

MIT. See `LICENSE.txt`.
