#include "UI/UIHost.hpp"

#include <string>

#include "Core/Error.hpp"
#include "Core/LogMacros.hpp"
#include "Core/Result.hpp"
#include "UI/ImGui/ImGuiWin32MessageHook.hpp"

namespace ellindyer::ui
{

UIHost::UIHost()
    : window_(std::make_unique<ellindyer::ui::platform::PlatformWindow>())
    , graphics_(std::make_unique<ellindyer::ui::graphics::DirectX11Context>())
    , imgui_(std::make_unique<ellindyer::ui::imgui_layer::ImGuiLayer>())
    , logo_(std::make_unique<ellindyer::ui::branding::LogoTexture>())
{
}

UIHost::~UIHost()
{
    Shutdown();
}

ellindyer::core::Result<void> UIHost::Initialize(const UIHostDescription& description)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (initialized_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "UIHost is already initialized."));
    }

    ellindyer::ui::platform::PlatformWindowDescription window_description{};
    window_description.title      = description.window_title;
    window_description.width      = description.window_width;
    window_description.height     = description.window_height;
    window_description.resizable  = description.resizable;
    window_description.centered   = description.centered;

    const Result<void> window_result = window_->Create(window_description);
    if (window_result.HasError())
    {
        return window_result;
    }

    window_->SetResizeCallback(
        [this](std::uint32_t width, std::uint32_t height)
        {
            OnWindowResize(width, height);
        });

    ellindyer::ui::graphics::DirectX11ContextDescription graphics_description{};
    graphics_description.window  = window_->GetHandle();
    graphics_description.width   = window_->GetWidth();
    graphics_description.height  = window_->GetHeight();
    graphics_description.vsync   = description.vsync;

    const Result<void> graphics_result = graphics_->Create(graphics_description);
    if (graphics_result.HasError())
    {
        window_->Destroy();
        return graphics_result;
    }

    ellindyer::ui::imgui_layer::ImGuiLayerDescription imgui_description{};
    imgui_description.window   = window_.get();
    imgui_description.graphics = graphics_.get();

    const Result<void> imgui_result = imgui_->Initialize(imgui_description);
    if (imgui_result.HasError())
    {
        graphics_->Destroy();
        window_->Destroy();
        return imgui_result;
    }

    ResolveBranding();
    LoadFonts();
    LoadLogo();

    initialized_ = true;
    return Result<void>{};
}

void UIHost::Shutdown()
{
    if (!initialized_ && !window_)
    {
        return;
    }

    if (logo_)
    {
        logo_->Release();
    }

    if (imgui_)
    {
        imgui_->Shutdown();
    }

    if (graphics_)
    {
        graphics_->Destroy();
    }

    if (window_)
    {
        window_->Destroy();
    }

    initialized_ = false;
}

bool UIHost::IsInitialized() const noexcept
{
    return initialized_;
}

void UIHost::ShowWindow()
{
    if (window_)
    {
        window_->Show();
    }
}

bool UIHost::PumpMessages()
{
    if (!window_)
    {
        return false;
    }

    MSG message{};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            window_->RequestClose();
            return false;
        }

        if (ellindyer::ui::imgui_layer::ImGuiWin32MessageHook::HandleMessage(
                message.hwnd, message.message, message.wParam, message.lParam))
        {
            continue;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return true;
}

bool UIHost::ShouldClose() const noexcept
{
    return window_ ? window_->ShouldClose() : true;
}

void UIHost::RequestClose()
{
    if (window_)
    {
        window_->RequestClose();
    }
}

void UIHost::BeginFrame()
{
    if (!initialized_)
    {
        return;
    }

    graphics_->BeginFrame(0.06f, 0.06f, 0.06f, 1.0f);
    imgui_->BeginFrame();
}

void UIHost::EndFrame()
{
    if (!initialized_)
    {
        return;
    }
    imgui_->EndFrame();
}

ellindyer::core::Result<void> UIHost::Present()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (!initialized_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "UIHost is not initialized."));
    }

    const Result<void> render_result = imgui_->Render();
    if (render_result.HasError())
    {
        return render_result;
    }

    return graphics_->Present();
}

std::uint32_t UIHost::GetClientWidth() const noexcept
{
    return window_ ? window_->GetWidth() : 0U;
}

std::uint32_t UIHost::GetClientHeight() const noexcept
{
    return window_ ? window_->GetHeight() : 0U;
}

HWND UIHost::GetNativeHandle() const noexcept
{
    return window_ ? window_->GetHandle() : nullptr;
}

ellindyer::ui::graphics::DirectX11Context& UIHost::GetGraphics() noexcept
{
    return *graphics_;
}

const ellindyer::ui::branding::BrandingAssets& UIHost::GetBrandingAssets() const noexcept
{
    return branding_;
}

const ellindyer::ui::fonts::FontSet& UIHost::GetFonts() const noexcept
{
    return fonts_;
}

ellindyer::ui::branding::LogoTexture& UIHost::GetLogo() noexcept
{
    return *logo_;
}

void UIHost::OnWindowResize(std::uint32_t width, std::uint32_t height)
{
    if (graphics_ && width > 0 && height > 0)
    {
        const ellindyer::core::Result<void> resize_result = graphics_->Resize(width, height);
        (void)resize_result;
    }
}

void UIHost::ResolveBranding()
{
    branding_ = ellindyer::ui::branding::BrandingAssetsLocator::Resolve();

    const std::string description = ellindyer::ui::branding::BrandingAssetsLocator::Describe(branding_);
    ELLINDYER_LOG_INFO("Branding assets resolved:\n" + description);
}

void UIHost::LoadFonts()
{
    ellindyer::ui::fonts::FontLoadOptions options{};
    fonts_ = ellindyer::ui::fonts::FontManager::Load(branding_, options);

    const std::string description = ellindyer::ui::fonts::FontManager::Describe(fonts_);
    ELLINDYER_LOG_INFO("Fonts loaded: " + description);
}

void UIHost::LoadLogo()
{
    const ellindyer::core::Result<void> logo_result = logo_->Load(*graphics_, branding_);
    if (logo_result.HasError())
    {
        ELLINDYER_LOG_WARNING("Logo not loaded: " + logo_result.GetError().ToDiagnosticString());
        return;
    }
    ELLINDYER_LOG_INFO("Logo loaded");
}

} // namespace ellindyer::ui
