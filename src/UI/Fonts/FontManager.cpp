#include "UI/Fonts/FontManager.hpp"

#include <imgui.h>

namespace ellindyer::ui::fonts
{

namespace
{

constexpr const char* kGlyphRangesHint = "default";

ImFont* TryLoadFont(const std::filesystem::path& path, float size)
{
    if (path.empty())
    {
        return nullptr;
    }

    ImFontConfig configuration{};
    configuration.OversampleH = 2;
    configuration.OversampleV = 2;
    configuration.PixelSnapH  = true;

    ImGuiIO& io = ImGui::GetIO();

    const std::string path_text = path.generic_string();
    ImFont* font = io.Fonts->AddFontFromFileTTF(path_text.c_str(),
                                                size,
                                                &configuration,
                                                io.Fonts->GetGlyphRangesDefault());
    return font;
}

ImFont* EnsureFallbackFont(float size)
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig configuration{};
    configuration.SizePixels = size;

    return io.Fonts->AddFontDefault(&configuration);
}

} // namespace

FontSet FontManager::Load(const branding::BrandingAssets& assets,
                          const FontLoadOptions& options)
{
    FontSet font_set{};

    ImFont* regular = TryLoadFont(assets.font_regular_path, options.default_size);
    ImFont* small   = TryLoadFont(assets.font_regular_path, options.small_size);
    ImFont* large   = TryLoadFont(assets.font_regular_path, options.large_size);
    ImFont* medium  = TryLoadFont(assets.font_medium_path,  options.title_size);
    ImFont* bold    = TryLoadFont(assets.font_bold_path,    options.heading_size);
    ImFont* light   = TryLoadFont(assets.font_light_path,   options.subtle_size);

    font_set.using_roboto = (regular != nullptr);

    if (regular == nullptr)
    {
        regular = EnsureFallbackFont(options.default_size);
    }
    if (small == nullptr)
    {
        small = regular;
    }
    if (large == nullptr)
    {
        large = regular;
    }
    if (medium == nullptr)
    {
        medium = regular;
    }
    if (bold == nullptr)
    {
        bold = regular;
    }
    if (light == nullptr)
    {
        light = regular;
    }

    font_set.default_regular = regular;
    font_set.small_regular   = small;
    font_set.large_regular   = large;
    font_set.title_medium    = medium;
    font_set.heading_bold    = bold;
    font_set.subtle_light    = light;

    return font_set;
}

std::string FontManager::Describe(const FontSet& font_set)
{
    std::string result;
    result += "regular: ";
    result += (font_set.default_regular != nullptr ? "ok" : "missing");
    result += ", small: ";
    result += (font_set.small_regular != nullptr ? "ok" : "missing");
    result += ", large: ";
    result += (font_set.large_regular != nullptr ? "ok" : "missing");
    result += ", medium: ";
    result += (font_set.title_medium != nullptr ? "ok" : "missing");
    result += ", bold: ";
    result += (font_set.heading_bold != nullptr ? "ok" : "missing");
    result += ", light: ";
    result += (font_set.subtle_light != nullptr ? "ok" : "missing");
    result += ", source: ";
    result += (font_set.using_roboto ? "Roboto" : "ImGui default");
    return result;
}

} // namespace ellindyer::ui::fonts
