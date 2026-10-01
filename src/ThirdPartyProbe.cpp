#include "ThirdPartyProbe.hpp"

#if defined(__has_include)
#    if __has_include(<nlohmann/json.hpp>)
#        define ELLINDYER_HAS_JSON 1
#    else
#        define ELLINDYER_HAS_JSON 0
#    endif
#    if __has_include(<spdlog/spdlog.h>)
#        define ELLINDYER_HAS_SPDLOG 1
#    else
#        define ELLINDYER_HAS_SPDLOG 0
#    endif
#    if __has_include(<stb/stb_image.h>)
#        define ELLINDYER_HAS_STB 1
#    else
#        define ELLINDYER_HAS_STB 0
#    endif
#    if __has_include(<imgui/imgui.h>)
#        define ELLINDYER_HAS_IMGUI 1
#    else
#        define ELLINDYER_HAS_IMGUI 0
#    endif
#else
#    define ELLINDYER_HAS_JSON 0
#    define ELLINDYER_HAS_SPDLOG 0
#    define ELLINDYER_HAS_STB 0
#    define ELLINDYER_HAS_IMGUI 0
#endif

namespace ellindyer::third_party
{

LibraryStatus QueryLibraryStatus() noexcept
{
    LibraryStatus status{};
    status.json_available   = (ELLINDYER_HAS_JSON   != 0);
    status.spdlog_available = (ELLINDYER_HAS_SPDLOG != 0);
    status.stb_available    = (ELLINDYER_HAS_STB    != 0);
    status.imgui_available  = (ELLINDYER_HAS_IMGUI  != 0);
    return status;
}

} // namespace ellindyer::third_party
