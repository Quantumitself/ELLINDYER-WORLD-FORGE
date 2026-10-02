#pragma once

#include <cstdint>
#include <string>

#include "UI/StatusBar/StatusBar.hpp"

namespace ellindyer::ui::statusbar
{

struct ApplicationStatusBarState
{
    std::string   message              = "Ready";
    std::uint32_t entity_count         = 0;
    std::uint32_t diagnostic_errors    = 0;
    std::uint32_t diagnostic_warnings  = 0;
    float         frame_time_ms        = 0.0f;
    float         frames_per_second    = 0.0f;
    bool          has_project          = false;
    std::string   project_name;
    std::string   selection_summary;
};

class ApplicationStatusBarBuilder
{
public:
    ApplicationStatusBarBuilder() = delete;

    static void Build(StatusBar& status_bar,
                      const ApplicationStatusBarState& state);

    static void Update(StatusBar& status_bar,
                       const ApplicationStatusBarState& state);
};

} // namespace ellindyer::ui::statusbar
