cmake_minimum_required(VERSION 3.20)

project(EllindyerWorldForge
    VERSION 0.1.0
    DESCRIPTION "World & Game Design Studio"
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

foreach(cfg IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/lib")
endforeach()

# -----------------------------------------------------------------------------
# Compiler options
# -----------------------------------------------------------------------------

if(MSVC)
    add_compile_options(
        /W4
        /permissive-
        /Zc:__cplusplus
        /Zc:preprocessor
        /utf-8
        /MP
    )
    add_compile_definitions(
        UNICODE
        _UNICODE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(/O2 /Oi /GL)
        add_link_options(/LTCG)
    endif()
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
    )
    add_compile_definitions(
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-O3)
    endif()
endif()

# -----------------------------------------------------------------------------
# Project version definitions
# -----------------------------------------------------------------------------

set(ELLINDYER_WORLD_FORGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ELLINDYER_WORLD_FORGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ELLINDYER_WORLD_FORGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# -----------------------------------------------------------------------------
# Source tree
# -----------------------------------------------------------------------------

set(ELLINDYER_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(ELLINDYER_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(ELLINDYER_DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

set(ELLINDYER_APP_TARGET "EllindyerWorldForge")

# -----------------------------------------------------------------------------
# Third-party libraries
# -----------------------------------------------------------------------------

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party"
                 "${CMAKE_BINARY_DIR}/third_party")

# -----------------------------------------------------------------------------
# Application executable
# -----------------------------------------------------------------------------

set(ELLINDYER_APP_SOURCES
    "${ELLINDYER_SOURCES_DIR}/main.cpp"
    "${ELLINDYER_SOURCES_DIR}/ThirdPartyProbe.cpp"
    "${ELLINDYER_SOURCES_DIR}/App/ApplicationContext.cpp"
    "${ELLINDYER_SOURCES_DIR}/App/ApplicationLifecycle.cpp"
    "${ELLINDYER_SOURCES_DIR}/App/Application.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Platform/PlatformWindow.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Graphics/DirectX11Context.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/ImGui/ImGuiLayer.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/ImGui/ImGuiWin32MessageHook.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/UIHost.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Branding/BrandingAssets.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Branding/LogoTexture.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Fonts/FontManager.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Textures/TextureLoader.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Splash/SplashScreen.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Splash/SplashOrchestrator.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Shell/ShellWindow.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Shell/ShellLayout.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Layout/Panel.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Layout/PanelLayout.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Panels/ProjectExplorerPanel.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Panels/WorkspacePanel.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Panels/InspectorPanel.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Panels/StatusBarPanel.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Menu/MenuItem.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Menu/MenuBar.cpp"
    "${ELLINDYER_SOURCES_DIR}/UI/Menu/ApplicationMenuBuilder.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/Version.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/BuildInfo.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/ApplicationPaths.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/ApplicationInfo.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/Identifier.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/IdentifierGenerator.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/IdentifierFormatter.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/ErrorCode.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/Error.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/ErrorLogging.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/LogLevel.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/Logger.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/LogConfiguration.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/LogBootstrap.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/LoggerExtensions.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/FileError.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/FileHelpers.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/PathHelpers.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/FileWatcher.cpp"
    "${ELLINDYER_SOURCES_DIR}/Core/FileSystemSummary.cpp"
)

add_executable(${ELLINDYER_APP_TARGET}
    ${ELLINDYER_APP_SOURCES}
)

target_include_directories(${ELLINDYER_APP_TARGET}
    PRIVATE
        "${ELLINDYER_SOURCES_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog/include"
)

target_compile_definitions(${ELLINDYER_APP_TARGET}
    PRIVATE
        ELLINDYER_WORLD_FORGE_VERSION_MAJOR=${ELLINDYER_WORLD_FORGE_VERSION_MAJOR}
        ELLINDYER_WORLD_FORGE_VERSION_MINOR=${ELLINDYER_WORLD_FORGE_VERSION_MINOR}
        ELLINDYER_WORLD_FORGE_VERSION_PATCH=${ELLINDYER_WORLD_FORGE_VERSION_PATCH}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${ELLINDYER_APP_TARGET} PRIVATE ELLINDYER_BUILD_DEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${ELLINDYER_APP_TARGET} PRIVATE ELLINDYER_BUILD_RELEASE=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(${ELLINDYER_APP_TARGET} PRIVATE ELLINDYER_BUILD_RELWITHDEBINFO=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_definitions(${ELLINDYER_APP_TARGET} PRIVATE ELLINDYER_BUILD_MINSIZEREL=1)
endif()

if(CMAKE_CONFIGURATION_TYPES)
    foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${cfg}" cfg_upper)
        target_compile_definitions(${ELLINDYER_APP_TARGET} PRIVATE
            $<$<CONFIG:${cfg}>:ELLINDYER_BUILD_${cfg_upper}=1>
        )
    endforeach()
endif()

target_link_libraries(${ELLINDYER_APP_TARGET}
    PRIVATE
        ellindyer_json
        ellindyer_spdlog
        ellindyer_stb
        ellindyer_imgui
        ellindyer_catch2
)

if(WIN32)
    target_link_libraries(${ELLINDYER_APP_TARGET} PRIVATE shell32 ole32 user32 gdi32)
endif()

set_target_properties(${ELLINDYER_APP_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "EllindyerWorldForge"
)

if(WIN32)
    set_target_properties(${ELLINDYER_APP_TARGET} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Disable MSBuild's vcpkg auto-integration for this target.
if(MSVC)
    set_target_properties(${ELLINDYER_APP_TARGET} PROPERTIES
        VS_GLOBAL_VcpkgEnabled "false"
        VS_GLOBAL_VcpkgEnableManifest "false"
        VS_GLOBAL_VcpkgAutoLink "false"
        VS_GLOBAL_VcpkgConfiguration "Debug"
        VS_GLOBAL_VcpkgEnableLock "false"
    )
endif()

if(MSVC)
    set_target_properties(${ELLINDYER_APP_TARGET} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# -----------------------------------------------------------------------------
# Resource copy
# -----------------------------------------------------------------------------

add_custom_command(TARGET ${ELLINDYER_APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${ELLINDYER_APP_TARGET}>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ELLINDYER_RESOURCES_DIR}"
        "$<TARGET_FILE_DIR:${ELLINDYER_APP_TARGET}>/resources"
    COMMENT "Copying resources next to executable"
)

# -----------------------------------------------------------------------------
# Documentation target (optional)
# -----------------------------------------------------------------------------

add_custom_target(EllindyerWorldForge_Docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation is in ${ELLINDYER_DOCS_DIR}"
    COMMENT "Documentation location"
)
