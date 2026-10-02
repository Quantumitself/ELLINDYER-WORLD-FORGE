#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace ellindyer::ui::statusbar
{

enum class StatusBarSegmentKind : std::uint8_t
{
    Text,
    Message,
    Counter,
    Progress,
    Separator,
    Spacer
};

struct StatusBarSegment
{
    StatusBarSegmentKind kind = StatusBarSegmentKind::Text;
    std::string          identifier;
    std::string          text;
    std::string          tooltip;
    float                progress_value   = 0.0f;
    float                spacer_width     = 8.0f;
    bool                 visible          = true;
};

[[nodiscard]] StatusBarSegment MakeTextSegment(std::string identifier,
                                               std::string text,
                                               std::string tooltip = std::string{});

[[nodiscard]] StatusBarSegment MakeMessageSegment(std::string identifier,
                                                  std::string text);

[[nodiscard]] StatusBarSegment MakeCounterSegment(std::string identifier,
                                                  std::string label,
                                                  std::uint64_t value);

[[nodiscard]] StatusBarSegment MakeProgressSegment(std::string identifier,
                                                   float progress_value,
                                                   std::string tooltip = std::string{});

[[nodiscard]] StatusBarSegment MakeSeparatorSegment(std::string identifier);

[[nodiscard]] StatusBarSegment MakeSpacerSegment(std::string identifier,
                                                 float width);

void SetSegmentText(StatusBarSegment& segment, std::string text);

void SetSegmentCounter(StatusBarSegment& segment,
                       std::string label,
                       std::uint64_t value);

void SetSegmentProgress(StatusBarSegment& segment, float progress_value);

void SetSegmentVisible(StatusBarSegment& segment, bool visible) noexcept;

} // namespace ellindyer::ui::statusbar
