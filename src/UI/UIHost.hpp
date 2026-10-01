#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <windows.h>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Branding/BrandingAssets.hpp"
#include "UI/Branding/LogoTexture.hpp"
#include "UI/Fonts/FontManager.hpp"
#include "UI/Graphics/DirectX11Context.hpp"
#include "UI/ImGui/ImGuiLayer.hpp"
#include "UI/Platform/PlatformWindow.hpp"

namespace ellindyer::ui
{

struct UIHostDescription
{
    std::wstring  window_title  = L"Ellindyer World Forge";
    std::uint32_t window_width  = 1400U;
    std::uint32_t window_height = 900U;
    bool          vsync         = true;
    bool          centered      = true;
    bool          resizable     = true;
};

class UIHost
{
public:
    UIHost();
    ~UIHost();

    UIHost(const UIHost&) = delete;
    UIHost& operator=(const UIHost&) = delete;
    UIHost(UIHost&&) noexcept = delete;
    UIHost& operator=(UIHost&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Initialize(
        const UIHostDescription& description);

    void Shutdown();

    [[nodiscard]] bool IsInitialized() const noexcept;

    void ShowWindow();

    [[nodiscard]] bool PumpMessages();

    [[nodiscard]] bool ShouldClose() const noexcept;

    void RequestClose();

    void BeginFrame();

    void EndFrame();

    [[nodiscard]] ellindyer::core::Result<void> Present();

    [[nodiscard]] std::uint32_t GetClientWidth() const noexcept;

    [[nodiscard]] std::uint32_t GetClientHeight() const noexcept;

    [[nodiscard]] HWND GetNativeHandle() const noexcept;

    [[nodiscard]] ellindyer::ui::graphics::DirectX11Context& GetGraphics() noexcept;

    [[nodiscard]] const ellindyer::ui::branding::BrandingAssets& GetBrandingAssets() const noexcept;

    [[nodiscard]] const ellindyer::ui::fonts::FontSet& GetFonts() const noexcept;

    [[nodiscard]] ellindyer::ui::branding::LogoTexture& GetLogo() noexcept;

private:
    void OnWindowResize(std::uint32_t width, std::uint32_t height);

    void ResolveBranding();

    void LoadFonts();

    void LoadLogo();

    std::unique_ptr<ellindyer::ui::platform::PlatformWindow>   window_;
    std::unique_ptr<ellindyer::ui::graphics::DirectX11Context> graphics_;
    std::unique_ptr<ellindyer::ui::imgui_layer::ImGuiLayer>    imgui_;

    ellindyer::ui::branding::BrandingAssets        branding_{};
    ellindyer::ui::fonts::FontSet                  fonts_{};
    std::unique_ptr<ellindyer::ui::branding::LogoTexture> logo_;

    bool initialized_ = false;
};

} // namespace ellindyer::ui
