#pragma once

#include <functional>
#include <string>

#include "UI/Toolbar/Toolbar.hpp"

namespace ellindyer::ui::toolbar
{

struct ApplicationToolbarHandlers
{
    std::function<void()> on_new_project;
    std::function<void()> on_open_project;
    std::function<void()> on_save_project;

    std::function<void()> on_undo;
    std::function<void()> on_redo;

    std::function<void()> on_toggle_project_explorer;
    std::function<void()> on_toggle_inspector;

    std::function<void()> on_validate_project;

    std::function<void()> on_open_search;

    std::function<void()> on_export_universal;
};

struct ApplicationToolbarState
{
    bool has_project            = false;
    bool can_undo               = false;
    bool can_redo               = false;
    bool show_project_explorer  = true;
    bool show_inspector         = true;
};

class ApplicationToolbarBuilder
{
public:
    ApplicationToolbarBuilder() = delete;

    static void Build(Toolbar& toolbar,
                      const ApplicationToolbarState& state,
                      const ApplicationToolbarHandlers& handlers);
};

} // namespace ellindyer::ui::toolbar
