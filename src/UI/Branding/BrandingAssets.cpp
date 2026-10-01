#include "UI/Branding/BrandingAssets.hpp"

#include <sstream>

#include "Core/ApplicationPaths.hpp"

namespace ellindyer::ui::branding
{

namespace
{

constexpr const char* kLogoFileName        = "app_logo.png";
constexpr const char* kFontRegularFileName = "Roboto-Regular.ttf";
constexpr const char* kFontMediumFileName  = "Roboto-Medium.ttf";
constexpr const char* kFontBoldFileName    = "Roboto-Bold.ttf";
constexpr const char* kFontLightFileName   = "Roboto-Light.ttf";
constexpr const char* kFontDirectoryName   = "Roboto";

} // namespace

BrandingAssets BrandingAssetsLocator::Resolve()
{
    using ellindyer::core::ApplicationPaths;

    BrandingAssets assets{};

    const std::filesystem::path icons_directory = ApplicationPaths::GetIconsDirectory();
    if (!icons_directory.empty())
    {
        const std::filesystem::path candidate = icons_directory / kLogoFileName;
        std::error_code ec;
        if (std::filesystem::exists(candidate, ec) && !ec)
        {
            assets.logo_path = candidate;
        }
    }

    const std::filesystem::path fonts_root = ApplicationPaths::GetFontsDirectory();
    if (!fonts_root.empty())
    {
        const std::filesystem::path fonts_directory = fonts_root / kFontDirectoryName;

        const auto resolve_font = [&fonts_directory](const char* file_name) -> std::filesystem::path
        {
            const std::filesystem::path candidate = fonts_directory / file_name;
            std::error_code ec;
            if (std::filesystem::exists(candidate, ec) && !ec)
            {
                return candidate;
            }
            return {};
        };

        assets.font_regular_path = resolve_font(kFontRegularFileName);
        assets.font_medium_path  = resolve_font(kFontMediumFileName);
        assets.font_bold_path    = resolve_font(kFontBoldFileName);
        assets.font_light_path   = resolve_font(kFontLightFileName);
    }

    return assets;
}

std::string BrandingAssetsLocator::Describe(const BrandingAssets& assets)
{
    std::ostringstream stream;
    stream << "logo: "       << (assets.logo_path.empty()        ? "missing" : assets.logo_path.generic_string()) << '\n';
    stream << "regular: "    << (assets.font_regular_path.empty()? "missing" : assets.font_regular_path.generic_string()) << '\n';
    stream << "medium: "     << (assets.font_medium_path.empty() ? "missing" : assets.font_medium_path.generic_string()) << '\n';
    stream << "bold: "       << (assets.font_bold_path.empty()   ? "missing" : assets.font_bold_path.generic_string()) << '\n';
    stream << "light: "      << (assets.font_light_path.empty()  ? "missing" : assets.font_light_path.generic_string());
    return stream.str();
}

} // namespace ellindyer::ui::branding
