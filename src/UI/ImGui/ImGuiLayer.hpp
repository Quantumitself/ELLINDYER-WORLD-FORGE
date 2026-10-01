#pragma once

#include <cstdint>

#include <windows.h>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Graphics/DirectX11Context.hpp"
#include "UI/Platform/PlatformWindow.hpp"

namespace ellindyer::ui::imgui_layer
{

struct ImGuiLayerDescription
{
    ellindyer::ui::platform::PlatformWindow*      window  = nullptr;
    ellindyer::ui::graphics::DirectX11Context*    graphics = nullptr;
    bool enable_ini_file = false;
    bool enable_keyboard_navigation = true;
};

class ImGuiLayer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;
    ImGuiLayer(ImGuiLayer&&) noexcept = delete;
    ImGuiLayer& operator=(ImGuiLayer&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Initialize(
        const ImGuiLayerDescription& description);

    void Shutdown();

    [[nodiscard]] bool IsInitialized() const noexcept;

    void BeginFrame();

    void EndFrame();

    [[nodiscard]] ellindyer::core::Result<void> Render();

    [[nodiscard]] bool WantsCaptureMouse() const noexcept;

    [[nodiscard]] bool WantsCaptureKeyboard() const noexcept;

private:
    void ApplyStyle();

    ellindyer::ui::platform::PlatformWindow*   window_    = nullptr;
    ellindyer::ui::graphics::DirectX11Context* graphics_  = nullptr;
    bool                                       initialized_ = false;
};

} // namespace ellindyer::ui::imgui_layer
