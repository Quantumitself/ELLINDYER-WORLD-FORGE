#pragma once

#include <cstdint>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Shell/ShellWindow.hpp"

namespace ellindyer::ui::shell
{

struct ShellLayoutConfiguration
{
    float left_panel_width_fraction  = 0.20f;
    float right_panel_width_fraction = 0.22f;
    float left_panel_min_width       = 220.0f;
    float right_panel_min_width      = 260.0f;
    float center_min_width           = 320.0f;
};

class ShellLayout
{
public:
    ShellLayout();
    ~ShellLayout();

    ShellLayout(const ShellLayout&) = delete;
    ShellLayout& operator=(const ShellLayout&) = delete;
    ShellLayout(ShellLayout&&) noexcept = delete;
    ShellLayout& operator=(ShellLayout&&) noexcept = delete;

    void Configure(const ShellLayoutConfiguration& configuration);

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    [[nodiscard]] float GetLeftPanelWidth() const noexcept;

    [[nodiscard]] float GetRightPanelWidth() const noexcept;

    [[nodiscard]] float GetCenterWidth() const noexcept;

    [[nodiscard]] float GetContentHeight() const noexcept;

private:
    void RenderLeftPanel(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderCenterPanel(const ellindyer::ui::fonts::FontSet& fonts);

    void RenderRightPanel(const ellindyer::ui::fonts::FontSet& fonts);

    void ComputePanelWidths(const ImVec2& content_size);

    ShellLayoutConfiguration configuration_{};
    float                   left_panel_width_  = 0.0f;
    float                   right_panel_width_ = 0.0f;
    float                   center_width_      = 0.0f;
    float                   content_height_    = 0.0f;
};

} // namespace ellindyer::ui::shell
