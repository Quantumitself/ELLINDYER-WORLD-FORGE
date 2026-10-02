#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "UI/Docking/Splitter.hpp"

namespace ellindyer::ui::docking
{

enum class DockZone : std::uint8_t
{
    Left,
    Center,
    Right,
    Bottom
};

struct DockPanelStyle
{
    ImVec4  background_color = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    ImVec4  border_color     = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    ImVec4  title_color      = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
    float   padding_x        = 10.0f;
    float   padding_y        = 10.0f;
    float   border_size      = 1.0f;
    float   header_height    = 22.0f;
};

class DockPanel
{
public:
    using ContentRenderer = std::function<void()>;
    using HeaderRenderer  = std::function<void()>;

    DockPanel() = default;
    DockPanel(std::string name, std::string title);

    DockPanel(const DockPanel&) = delete;
    DockPanel& operator=(const DockPanel&) = delete;
    DockPanel(DockPanel&&) noexcept = default;
    DockPanel& operator=(DockPanel&&) noexcept = default;

    [[nodiscard]] const std::string& GetName() const noexcept;

    [[nodiscard]] const std::string& GetTitle() const noexcept;

    void SetTitle(std::string title);

    void SetStyle(const DockPanelStyle& style);

    [[nodiscard]] const DockPanelStyle& GetStyle() const noexcept;

    void SetContentRenderer(ContentRenderer renderer);

    void SetHeaderRenderer(HeaderRenderer renderer);

    void SetVisible(bool visible) noexcept;

    [[nodiscard]] bool IsVisible() const noexcept;

    void SetMinWidth(float min_width) noexcept;

    void SetMinHeight(float min_height) noexcept;

    [[nodiscard]] float GetMinWidth() const noexcept;

    [[nodiscard]] float GetMinHeight() const noexcept;

    void Render(float width, float height);

private:
    std::string       name_;
    std::string       title_;
    DockPanelStyle    style_{};
    ContentRenderer   content_renderer_;
    HeaderRenderer    header_renderer_;
    float             min_width_  = 0.0f;
    float             min_height_ = 0.0f;
    bool              visible_    = true;
};

} // namespace ellindyer::ui::docking
