#pragma once

#include <filesystem>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Branding/BrandingAssets.hpp"

struct ImFont;

namespace ellindyer::ui::fonts
{

struct FontSet
{
    ImFont* default_regular = nullptr;
    ImFont* small_regular   = nullptr;
    ImFont* large_regular   = nullptr;
    ImFont* title_medium    = nullptr;
    ImFont* heading_bold    = nullptr;
    ImFont* subtle_light    = nullptr;
    bool    using_roboto    = false;
};

struct FontLoadOptions
{
    float default_size = 15.0f;
    float small_size   = 13.0f;
    float large_size   = 19.0f;
    float title_size   = 28.0f;
    float heading_size = 22.0f;
    float subtle_size  = 13.0f;
};

class FontManager
{
public:
    FontManager() = delete;

    [[nodiscard]] static FontSet Load(const branding::BrandingAssets& assets,
                                      const FontLoadOptions& options);

    [[nodiscard]] static std::string Describe(const FontSet& font_set);
};

} // namespace ellindyer::ui::fonts
