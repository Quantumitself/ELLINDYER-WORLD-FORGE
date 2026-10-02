#include "UI/StatusBar/StatusBarSegment.hpp"

#include <utility>

namespace ellindyer::ui::statusbar
{

StatusBarSegment MakeTextSegment(std::string identifier,
                                 std::string text,
                                 std::string tooltip)
{
    StatusBarSegment segment{};
    segment.kind       = StatusBarSegmentKind::Text;
    segment.identifier = std::move(identifier);
    segment.text       = std::move(text);
    segment.tooltip    = std::move(tooltip);
    return segment;
}

StatusBarSegment MakeMessageSegment(std::string identifier,
                                    std::string text)
{
    StatusBarSegment segment{};
    segment.kind       = StatusBarSegmentKind::Message;
    segment.identifier = std::move(identifier);
    segment.text       = std::move(text);
    return segment;
}

StatusBarSegment MakeCounterSegment(std::string identifier,
                                    std::string label,
                                    std::uint64_t value)
{
    StatusBarSegment segment{};
    segment.kind       = StatusBarSegmentKind::Counter;
    segment.identifier = std::move(identifier);
    segment.text       = std::move(label);
    segment.progress_value = static_cast<float>(value);
    return segment;
}

StatusBarSegment MakeProgressSegment(std::string identifier,
                                     float progress_value,
                                     std::string tooltip)
{
    StatusBarSegment segment{};
    segment.kind           = StatusBarSegmentKind::Progress;
    segment.identifier     = std::move(identifier);
    segment.progress_value = progress_value;
    segment.tooltip        = std::move(tooltip);
    return segment;
}

StatusBarSegment MakeSeparatorSegment(std::string identifier)
{
    StatusBarSegment segment{};
    segment.kind       = StatusBarSegmentKind::Separator;
    segment.identifier = std::move(identifier);
    return segment;
}

StatusBarSegment MakeSpacerSegment(std::string identifier, float width)
{
    StatusBarSegment segment{};
    segment.kind         = StatusBarSegmentKind::Spacer;
    segment.identifier   = std::move(identifier);
    segment.spacer_width = width > 0.0f ? width : 8.0f;
    return segment;
}

void SetSegmentText(StatusBarSegment& segment, std::string text)
{
    segment.text = std::move(text);
}

void SetSegmentCounter(StatusBarSegment& segment,
                       std::string label,
                       std::uint64_t value)
{
    segment.kind           = StatusBarSegmentKind::Counter;
    segment.text           = std::move(label);
    segment.progress_value = static_cast<float>(value);
}

void SetSegmentProgress(StatusBarSegment& segment, float progress_value)
{
    segment.kind           = StatusBarSegmentKind::Progress;
    segment.progress_value = progress_value;
}

void SetSegmentVisible(StatusBarSegment& segment, bool visible) noexcept
{
    segment.visible = visible;
}

} // namespace ellindyer::ui::statusbar
