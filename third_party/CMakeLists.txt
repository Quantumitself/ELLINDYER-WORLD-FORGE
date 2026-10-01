# -----------------------------------------------------------------------------
# Third-party header-only libraries
# -----------------------------------------------------------------------------
# Each library is exposed as an INTERFACE target. Libraries are vendored under
# this directory and must remain header-only. No package manager is used.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# nlohmann/json
# -----------------------------------------------------------------------------
add_library(ellindyer_json INTERFACE)
target_include_directories(ellindyer_json
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/json/single_include"
)
target_compile_definitions(ellindyer_json
    INTERFACE
        JSON_USE_IMPLICIT_CONVERSIONS=1
)

# -----------------------------------------------------------------------------
# spdlog
# -----------------------------------------------------------------------------
add_library(ellindyer_spdlog INTERFACE)
target_include_directories(ellindyer_spdlog
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include"
)

# -----------------------------------------------------------------------------
# stb (image loading)
# -----------------------------------------------------------------------------
set(ELLINDYER_STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stb")

if(EXISTS "${ELLINDYER_STB_DIR}/stb_image.h")
    add_library(ellindyer_stb STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image_impl.cpp"
    )
    target_include_directories(ellindyer_stb
        PUBLIC
            "${ELLINDYER_STB_DIR}"
    )
else()
    add_library(ellindyer_stb INTERFACE)
    target_include_directories(ellindyer_stb
        INTERFACE
            "${ELLINDYER_STB_DIR}"
    )
endif()

# -----------------------------------------------------------------------------
# Dear ImGui (compiled as an internal static library)
# -----------------------------------------------------------------------------
set(ELLINDYER_IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

if(EXISTS "${ELLINDYER_IMGUI_DIR}/imgui.cpp")
    add_library(ellindyer_imgui STATIC
        "${ELLINDYER_IMGUI_DIR}/imgui.cpp"
        "${ELLINDYER_IMGUI_DIR}/imgui_draw.cpp"
        "${ELLINDYER_IMGUI_DIR}/imgui_tables.cpp"
        "${ELLINDYER_IMGUI_DIR}/imgui_widgets.cpp"
        "${ELLINDYER_IMGUI_DIR}/backends/imgui_impl_win32.cpp"
        "${ELLINDYER_IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
    )

    target_include_directories(ellindyer_imgui
        PUBLIC
            "${ELLINDYER_IMGUI_DIR}"
            "${ELLINDYER_IMGUI_DIR}/backends"
    )

    target_compile_definitions(ellindyer_imgui
        PUBLIC
            IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
            IMGUI_DISABLE_OBSOLETE_KEYIO=1
    )

    if(WIN32)
        target_link_libraries(ellindyer_imgui PUBLIC d3d11 dxgi)
    endif()

    set_target_properties(ellindyer_imgui PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
else()
    add_library(ellindyer_imgui INTERFACE)
    target_include_directories(ellindyer_imgui
        INTERFACE
            "${ELLINDYER_IMGUI_DIR}"
            "${ELLINDYER_IMGUI_DIR}/backends"
    )
    target_compile_definitions(ellindyer_imgui
        INTERFACE
            IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
            IMGUI_DISABLE_OBSOLETE_KEYIO=1
    )
endif()

# -----------------------------------------------------------------------------
# Catch2 (single-header, v2)
# -----------------------------------------------------------------------------
add_library(ellindyer_catch2 INTERFACE)
target_include_directories(ellindyer_catch2
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/catch2/single_include"
)
target_compile_definitions(ellindyer_catch2
    INTERFACE
        CATCH_CONFIG_CPP17_STRING_VIEW=1
)
