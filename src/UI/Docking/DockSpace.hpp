#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include "UI/Docking/DockPanel.hpp"
#include "UI/Docking/Splitter.hpp"
#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::docking
{

struct DockSpaceLayout
{
    float left_fraction    = 0.20f;
    float right_fraction   = 0.22f;
    float bottom_fraction  = 0.24f;
    float left_min_width   = 220.0f;
    float right_min_width  = 260.0f;
    float bottom_min_height = 160.0f;
    float center_min_width = 320.0f;
    float center_min_height = 240.0f;
    float splitter_thickness = 6.0f;
};

class DockSpace
{
public:
    DockSpace();
    ~DockSpace();

    DockSpace(const DockSpace&) = delete;
    DockSpace& operator=(const DockSpace&) = delete;
    DockSpace(DockSpace&&) noexcept = delete;
    DockSpace& operator=(DockSpace&&) noexcept = delete;

    void SetLayout(const DockSpaceLayout& layout);

    [[nodiscard]] const DockSpaceLayout& GetLayout() const noexcept;

    void AddPanel(DockZone zone, std::shared_ptr<DockPanel> panel);

    void ClearPanels();

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    void SetZoneVisible(DockZone zone, bool visible) noexcept;

    [[nodiscard]] bool IsZoneVisible(DockZone zone) const noexcept;

    void ResetSplitters() noexcept;

    [[nodiscard]] float GetLeftWidth() const noexcept;

    [[nodiscard]] float GetRightWidth() const noexcept;

    [[nodiscard]] float GetCenterWidth() const noexcept;

    [[nodiscard]] float GetBottomHeight() const noexcept;

    [[nodiscard]] float GetCenterHeight() const noexcept;

private:
    struct PanelEntry
    {
        std::shared_ptr<DockPanel> panel;
        DockZone                   zone = DockZone::Center;
    };

    void ComputeLayout(const ImVec2& available,
                       float& left_width,
                       float& right_width,
                       float& center_width,
                       float& bottom_height,
                       float& center_height) const;

    void RenderZonePanels(DockZone zone,
                          float width,
                          float height,
                          const ellindyer::ui::fonts::FontSet& fonts);

    DockSpaceLayout              layout_{};
    std::vector<PanelEntry>      panels_;
    Splitter                     left_splitter_{};
    Splitter                     right_splitter_{};
    Splitter                     bottom_splitter_{};

    bool left_visible_   = true;
    bool right_visible_  = true;
    bool bottom_visible_ = true;
    bool center_visible_ = true;

    float last_left_width_    = 0.0f;
    float last_right_width_   = 0.0f;
    float last_center_width_  = 0.0f;
    float last_bottom_height_ = 0.0f;
    float last_center_height_ = 0.0f;
};

} // namespace ellindyer::ui::docking
