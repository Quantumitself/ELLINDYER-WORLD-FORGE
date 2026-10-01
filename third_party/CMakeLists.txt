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
add_library(ellindyer_stb INTERFACE)
target_include_directories(ellindyer_stb
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# -----------------------------------------------------------------------------
# Dear ImGui
# -----------------------------------------------------------------------------
add_library(ellindyer_imgui INTERFACE)
target_include_directories(ellindyer_imgui
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends"
)
target_compile_definitions(ellindyer_imgui
    INTERFACE
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
        IMGUI_DISABLE_OBSOLETE_KEYIO=1
)

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
