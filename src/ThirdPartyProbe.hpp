#pragma once

#include <string_view>

namespace ellindyer::third_party
{

struct LibraryStatus
{
    bool json_available;
    bool spdlog_available;
    bool stb_available;
    bool imgui_available;
};

[[nodiscard]] LibraryStatus QueryLibraryStatus() noexcept;

} // namespace ellindyer::third_party
