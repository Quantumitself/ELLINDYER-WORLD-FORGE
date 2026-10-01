#include "UI/Splash/SplashOrchestrator.hpp"

#include <string>

#include "Core/LogMacros.hpp"

namespace ellindyer::ui::splash
{

void SplashOrchestrator::InitializeBranding(ellindyer::ui::branding::BrandingAssets& branding_out)
{
    branding_out = ellindyer::ui::branding::BrandingAssetsLocator::Resolve();
}

void SplashOrchestrator::LoadFonts(ellindyer::ui::branding::BrandingAssets& assets,
                                   ellindyer::ui::fonts::FontSet& font_set_out)
{
    ellindyer::ui::fonts::FontLoadOptions options{};
    font_set_out = ellindyer::ui::fonts::FontManager::Load(assets, options);
}

bool SplashOrchestrator::LoadLogo(ellindyer::ui::graphics::DirectX11Context& graphics,
                                  ellindyer::ui::branding::BrandingAssets& assets,
                                  ellindyer::ui::branding::LogoTexture& logo_out,
                                  std::string& status_out)
{
    const ellindyer::core::Result<void> logo_result = logo_out.Load(graphics, assets);
    if (logo_result.HasError())
    {
        status_out = logo_result.GetError().ToDiagnosticString();
        ELLINDYER_LOG_WARNING(status_out);
        return false;
    }
    status_out = "Logo loaded.";
    return true;
}

} // namespace ellindyer::ui::splash
