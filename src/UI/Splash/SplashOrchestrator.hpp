#pragma once

#include <string>
#include <string_view>

#include "UI/Branding/BrandingAssets.hpp"
#include "UI/Branding/LogoTexture.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Graphics/DirectX11Context.hpp"
#include "UI/Splash/SplashScreen.hpp"

namespace ellindyer::ui::splash
{

struct SplashBootstrapResult
{
    ellindyer::ui::branding::BrandingAssets branding;
    ellindyer::ui::fonts::FontSet           fonts;
    ellindyer::ui::branding::LogoTexture    logo;
    std::string                             status_summary;
};

class SplashOrchestrator
{
public:
    SplashOrchestrator() = delete;

    static void InitializeBranding(ellindyer::ui::branding::BrandingAssets& branding_out);

    static void LoadFonts(ellindyer::ui::branding::BrandingAssets& assets,
                          ellindyer::ui::fonts::FontSet& font_set_out);

    static bool LoadLogo(ellindyer::ui::graphics::DirectX11Context& graphics,
                         ellindyer::ui::branding::BrandingAssets& assets,
                         ellindyer::ui::branding::LogoTexture& logo_out,
                         std::string& status_out);
};

} // namespace ellindyer::ui::splash
