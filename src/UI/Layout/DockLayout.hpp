#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include "UI/Fonts/FontManager.hpp"
#include "UI/Layout/Panel.hpp"
#include "UI/Layout/Splitter.hpp"

namespace ellindyer::ui::layout
{

struct DockSlot
{
    std::string name;
    Panel*      panel = nullptr;
    bool        visible = true;
    float       width_fraction  = 0.0f;  // used by Left/Right
    float       height_fraction = 0.0f;  // used by Bottom
    float       min_width       = 0.0f;
    float       min_height      = 0.0f;
    float       resolved_width  = 0.0f;
    float       resolved_height = 0.0f;
};

struct DockLayoutState
{
    float left_width       = 260.0f;
    float right_width      = 320.0f;
    float bottom_height    = 180.0f;
    bool  bottom_visible   = true;
    bool  left_visible     = true;
    bool  right_visible    = true;
    bool  center_visible   = true;
};

struct DockLayoutStyle
{
    float panel_spacing        = 0.0f;   // spacing handled by splitters
    float splitter_thickness   = 4.0f;
    float default_left_min     = 200.0f;
    float default_right_min    = 220.0f;
    float default_center_min   = 320.0f;
    float default_bottom_min   = 120.0f;
};

class DockLayout
{
public:
    using PanelRenderFn = void(*)(void* context,
                                  Panel& panel,
                                  float width,
                                  float height,
                                  const ellindyer::ui::fonts::FontSet& fonts);

    DockLayout();
    ~DockLayout();

    DockLayout(const DockLayout&) = delete;
    DockLayout& operator=(const DockLayout&) = delete;
    DockLayout(DockLayout&&) noexcept = delete;
    DockLayout& operator=(DockLayout&&) noexcept = delete;

    void SetStyle(const DockLayoutStyle& style);

    [[nodiscard]] const DockLayoutStyle& GetStyle() const noexcept;

    void SetLeftPanel(Panel* panel, float width, float min_width, bool visible);

    void SetRightPanel(Panel* panel, float width, float min_width, bool visible);

    void SetCenterPanel(Panel* panel, float min_width, bool visible);

    void SetBottomPanel(Panel* panel, float height, float min_height, bool visible);

    void SetLeftVisible(bool visible) noexcept;
    void SetRightVisible(bool visible) noexcept;
    void SetCenterVisible(bool visible) noexcept;
    void SetBottomVisible(bool visible) noexcept;

    [[nodiscard]] bool IsLeftVisible() const noexcept;
    [[nodiscard]] bool IsRightVisible() const noexcept;
    [[nodiscard]] bool IsCenterVisible() const noexcept;
    [[nodiscard]] bool IsBottomVisible() const noexcept;

    [[nodiscard]] float GetLeftWidth() const noexcept;
    [[nodiscard]] float GetRightWidth() const noexcept;
    [[nodiscard]] float GetBottomHeight() const noexcept;

    void ResetLayout() noexcept;

    // Renders the full docked layout in the current ImGui content region.
    // Each panel's content is produced by the corresponding render callback.
    void Render(const ellindyer::ui::fonts::FontSet& fonts,
                PanelRenderFn left_renderer,   void* left_context,
                PanelRenderFn right_renderer,  void* right_context,
                PanelRenderFn center_renderer, void* center_context,
                PanelRenderFn bottom_renderer, void* bottom_context);

private:
    void RenderSplitterHorizontal(Splitter& splitter,
                                  float splitter_thickness);

    void RenderSplitterVertical(Splitter& splitter,
                                float splitter_thickness);

    float ClampLeft(float width, float total_width) const noexcept;

    float ClampRight(float width, float total_width) const noexcept;

    float ClampBottom(float height, float total_height) const noexcept;

    DockLayoutStyle style_{};

    Panel* left_panel_   = nullptr;
    Panel* right_panel_  = nullptr;
    Panel* center_panel_ = nullptr;
    Panel* bottom_panel_ = nullptr;

    float  left_width_       = 260.0f;
    float  right_width_      = 320.0f;
    float  bottom_height_    = 180.0f;

    float  left_min_         = 200.0f;
    float  right_min_        = 220.0f;
    float  center_min_       = 320.0f;
    float  bottom_min_       = 120.0f;

    bool   left_visible_     = true;
    bool   right_visible_    = true;
    bool   center_visible_   = true;
    bool   bottom_visible_   = true;

    Splitter left_splitter_;
    Splitter right_splitter_;
    Splitter bottom_splitter_;
};

} // namespace ellindyer::ui::layout
