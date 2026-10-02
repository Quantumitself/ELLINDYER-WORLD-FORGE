#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "UI/Fonts/FontManager.hpp"
#include "UI/StatusBar/StatusBarSegment.hpp"

namespace ellindyer::ui::statusbar
{

struct StatusBarStyle
{
    float   height          = 26.0f;
    float   padding_x       = 12.0f;
    float   padding_y       = 4.0f;
    float   separator_width = 10.0f;
};

class StatusBar
{
public:
    using SegmentClickedHandler =
        std::function<void(const std::string& identifier)>;

    StatusBar();
    ~StatusBar();

    StatusBar(const StatusBar&) = delete;
    StatusBar& operator=(const StatusBar&) = delete;
    StatusBar(StatusBar&&) noexcept = delete;
    StatusBar& operator=(StatusBar&&) noexcept = delete;

    void Clear();

    void SetStyle(const StatusBarStyle& style);

    [[nodiscard]] const StatusBarStyle& GetStyle() const noexcept;

    void AddSegment(StatusBarSegment segment);

    void AddText(std::string identifier,
                 std::string text,
                 std::string tooltip = std::string{});

    void AddMessage(std::string identifier, std::string text);

    void AddCounter(std::string identifier,
                    std::string label,
                    std::uint64_t value);

    void AddProgress(std::string identifier,
                     float progress_value,
                     std::string tooltip = std::string{});

    void AddSeparator(std::string identifier);

    void AddSpacer(std::string identifier, float width);

    void SetSegmentText(const std::string& identifier, std::string text);

    void SetSegmentCounter(const std::string& identifier,
                           std::string label,
                           std::uint64_t value);

    void SetSegmentProgress(const std::string& identifier, float progress_value);

    void SetSegmentVisible(const std::string& identifier, bool visible);

    void SetSegmentClickedHandler(SegmentClickedHandler handler);

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

    [[nodiscard]] float GetHeight() const noexcept;

    [[nodiscard]] std::size_t GetSegmentCount() const noexcept;

    [[nodiscard]] const StatusBarSegment* FindSegment(const std::string& identifier) const noexcept;

private:
    void RenderSegment(const StatusBarSegment& segment,
                       const ellindyer::ui::fonts::FontSet& fonts,
                       bool& first_rendered);

    StatusBarSegment* FindSegmentMutable(const std::string& identifier);

    StatusBarStyle               style_{};
    std::vector<StatusBarSegment> segments_;
    SegmentClickedHandler        clicked_handler_;
};

} // namespace ellindyer::ui::statusbar
