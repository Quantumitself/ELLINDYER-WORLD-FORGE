#include "UI/StatusBar/ApplicationStatusBarBuilder.hpp"

#include <cstdio>

namespace ellindyer::ui::statusbar
{

namespace
{

constexpr const char* kMessageIdentifier     = "status.message";
constexpr const char* kProjectIdentifier     = "status.project";
constexpr const char* kSeparatorProjectId    = "status.sep.project";
constexpr const char* kEntitiesIdentifier    = "status.entities";
constexpr const char* kSeparatorEntitiesId   = "status.sep.entities";
constexpr const char* kDiagnosticsIdentifier = "status.diagnostics";
constexpr const char* kSelectionIdentifier   = "status.selection";
constexpr const char* kSeparatorRightId      = "status.sep.right";
constexpr const char* kFrameTimeIdentifier   = "status.frame_time";
constexpr const char* kSeparatorFrameTimeId  = "status.sep.frametime";
constexpr const char* kFpsIdentifier         = "status.fps";

std::string BuildEntitiesText(std::uint32_t entity_count)
{
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "Entities: %u",
                  static_cast<unsigned>(entity_count));
    return std::string(buffer);
}

std::string BuildDiagnosticsText(std::uint32_t errors, std::uint32_t warnings)
{
    char buffer[96] = {};
    std::snprintf(buffer, sizeof(buffer), "Diagnostics: %uE / %uW",
                  static_cast<unsigned>(errors),
                  static_cast<unsigned>(warnings));
    return std::string(buffer);
}

std::string BuildFrameTimeText(float frame_time_ms)
{
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "Frame: %.2f ms",
                  static_cast<double>(frame_time_ms));
    return std::string(buffer);
}

std::string BuildFpsText(float frames_per_second)
{
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "FPS: %.0f",
                  static_cast<double>(frames_per_second));
    return std::string(buffer);
}

} // namespace

void ApplicationStatusBarBuilder::Build(StatusBar& status_bar,
                                        const ApplicationStatusBarState& state)
{
    status_bar.Clear();

    StatusBarStyle style{};
    style.height          = 26.0f;
    style.padding_x       = 12.0f;
    style.padding_y       = 4.0f;
    style.separator_width = 12.0f;
    status_bar.SetStyle(style);

    status_bar.AddSegment(MakeMessageSegment(kMessageIdentifier, state.message));

    status_bar.AddSegment(MakeSeparatorSegment(kSeparatorProjectId));

    status_bar.AddSegment(MakeTextSegment(kProjectIdentifier,
                                          state.has_project
                                              ? state.project_name
                                              : std::string{"No project"}));

    status_bar.AddSegment(MakeSeparatorSegment(kSeparatorEntitiesId));

    status_bar.AddSegment(MakeTextSegment(kEntitiesIdentifier,
                                          BuildEntitiesText(state.entity_count),
                                          "Number of entities in the current project"));

    status_bar.AddSegment(MakeTextSegment(kDiagnosticsIdentifier,
                                          BuildDiagnosticsText(state.diagnostic_errors,
                                                               state.diagnostic_warnings),
                                          "Errors and warnings reported by validation"));

    status_bar.AddSegment(MakeTextSegment(kSelectionIdentifier,
                                          state.selection_summary,
                                          "Current selection"));

    status_bar.AddSegment(MakeSpacerSegment("status.spacer", 8.0f));

    status_bar.AddSegment(MakeSeparatorSegment(kSeparatorRightId));

    status_bar.AddSegment(MakeTextSegment(kFrameTimeIdentifier,
                                          BuildFrameTimeText(state.frame_time_ms),
                                          "Time spent rendering the previous frame"));

    status_bar.AddSegment(MakeSeparatorSegment(kSeparatorFrameTimeId));

    status_bar.AddSegment(MakeTextSegment(kFpsIdentifier,
                                          BuildFpsText(state.frames_per_second),
                                          "Frames per second"));
}

void ApplicationStatusBarBuilder::Update(StatusBar& status_bar,
                                         const ApplicationStatusBarState& state)
{
    status_bar.SetSegmentText(kMessageIdentifier, state.message);

    status_bar.SetSegmentText(kProjectIdentifier,
                              state.has_project ? state.project_name
                                                : std::string{"No project"});

    status_bar.SetSegmentText(kEntitiesIdentifier,
                              BuildEntitiesText(state.entity_count));

    status_bar.SetSegmentText(kDiagnosticsIdentifier,
                              BuildDiagnosticsText(state.diagnostic_errors,
                                                   state.diagnostic_warnings));

    status_bar.SetSegmentText(kSelectionIdentifier, state.selection_summary);

    status_bar.SetSegmentText(kFrameTimeIdentifier,
                              BuildFrameTimeText(state.frame_time_ms));

    status_bar.SetSegmentText(kFpsIdentifier,
                              BuildFpsText(state.frames_per_second));
}

} // namespace ellindyer::ui::statusbar
