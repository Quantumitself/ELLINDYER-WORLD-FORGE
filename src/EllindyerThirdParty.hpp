#pragma once

// -----------------------------------------------------------------------------
// Ellindyer Third-Party Aggregation Header
// -----------------------------------------------------------------------------
// Include this header to make all vendored third-party libraries available
// through a single include. Only aggregation is performed here; no custom
// wrappers or replacements are introduced.
// -----------------------------------------------------------------------------

#if defined(__has_include)
#    if __has_include(<nlohmann/json.hpp>)
#        include <nlohmann/json.hpp>
#    endif
#    if __has_include(<spdlog/spdlog.h>)
#        include <spdlog/spdlog.h>
#    endif
#    if __has_include(<stb/stb_image.h>)
#        include <stb/stb_image.h>
#    endif
#else
#    include <nlohmann/json.hpp>
#    include <spdlog/spdlog.h>
#endif
