# Dependencies

Ellindyer World Forge depends only on header-only third-party libraries.

## Vendored Libraries

| Library | Purpose | License |
|---|---|---|
| nlohmann/json | JSON serialization | MIT |
| spdlog | Logging | MIT |
| stb_image | Image loading | Public domain / MIT |
| Dear ImGui | UI framework | MIT |
| Catch2 v2 | Testing | BSL-1.0 |

## Rules

- No package manager is used.
- All libraries are vendored under `third_party/`.
- All libraries are header-only.
- Each library is exposed as a CMake INTERFACE target.
- Adding a new dependency requires a written justification.
