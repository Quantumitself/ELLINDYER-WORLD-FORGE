#include "UI/ImGui/ImGuiLayer.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace ellindyer::ui::imgui_layer
{

namespace
{

constexpr float kFontSizeDefault = 15.0f;

} // namespace

ImGuiLayer::ImGuiLayer() = default;

ImGuiLayer::~ImGuiLayer()
{
    Shutdown();
}

ellindyer::core::Result<void> ImGuiLayer::Initialize(
    const ImGuiLayerDescription& description)
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (initialized_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "ImGuiLayer is already initialized."));
    }

    if (description.window == nullptr || description.graphics == nullptr)
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "ImGuiLayer requires a window and a graphics context."));
    }

    window_   = description.window;
    graphics_ = description.graphics;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    if (!description.enable_ini_file)
    {
        io.IniFilename = nullptr;
    }

    ApplyStyle();

    if (!ImGui_ImplWin32_Init(window_->GetHandle()))
    {
        ImGui::DestroyContext();
        window_   = nullptr;
        graphics_ = nullptr;
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "ImGui_ImplWin32_Init failed."));
    }

    if (!ImGui_ImplDX11_Init(graphics_->GetDevice(), graphics_->GetDeviceContext()))
    {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        window_   = nullptr;
        graphics_ = nullptr;
        return Result<void>(MakeError(ErrorCode::PlatformError,
                                      "ImGui_ImplDX11_Init failed."));
    }

    initialized_ = true;
    return Result<void>{};
}

void ImGuiLayer::Shutdown()
{
    if (!initialized_)
    {
        return;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    window_      = nullptr;
    graphics_    = nullptr;
    initialized_ = false;
}

bool ImGuiLayer::IsInitialized() const noexcept
{
    return initialized_;
}

void ImGuiLayer::BeginFrame()
{
    if (!initialized_)
    {
        return;
    }
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndFrame()
{
    if (!initialized_)
    {
        return;
    }
    ImGui::EndFrame();
}

ellindyer::core::Result<void> ImGuiLayer::Render()
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    if (!initialized_)
    {
        return Result<void>(MakeError(ErrorCode::InvalidState,
                                      "ImGuiLayer is not initialized."));
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return Result<void>{};
}

bool ImGuiLayer::WantsCaptureMouse() const noexcept
{
    if (!initialized_)
    {
        return false;
    }
    return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiLayer::WantsCaptureKeyboard() const noexcept
{
    if (!initialized_)
    {
        return false;
    }
    return ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiLayer::ApplyStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding    = 0.0f;
    style.FrameRounding     = 0.0f;
    style.GrabRounding      = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding       = 0.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.WindowPadding     = ImVec2(6.0f, 6.0f);
    style.FramePadding      = ImVec2(6.0f, 3.0f);
    style.ItemSpacing       = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    colors[ImGuiCol_WindowBg]             = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Border]               = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_Button]               = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    colors[ImGuiCol_Header]               = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_Separator]            = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_SeparatorActive]      = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_ResizeGrip]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_Tab]                  = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered]           = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabSelected]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TabDimmed]            = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]    = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_DockingPreview]       = ImVec4(0.60f, 0.60f, 0.60f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]       = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_PlotLines]            = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]        = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]    = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_TableBorderLight]     = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TableRowBg]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
    colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.40f, 0.40f, 0.40f, 0.60f);
    colors[ImGuiCol_DragDropTarget]       = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    colors[ImGuiCol_NavCursor]            = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

} // namespace ellindyer::ui::imgui_layer
