#pragma once

#include <filesystem>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::ui::branding
{

struct BrandingAssets
{
    std::filesystem::path logo_path;
    std::filesystem::path font_regular_path;
    std::filesystem::path font_medium_path;
    std::filesystem::path font_bold_path;
    std::filesystem::path font_light_path;
};

class BrandingAssetsLocator
{
public:
    BrandingAssetsLocator() = delete;

    [[nodiscard]] static BrandingAssets Resolve();

    [[nodiscard]] static std::string Describe(const BrandingAssets& assets);
};

} // namespace ellindyer::ui::branding
