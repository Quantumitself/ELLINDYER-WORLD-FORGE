#pragma once

#include <functional>
#include <string>

#include "UI/Menu/MenuBar.hpp"

namespace ellindyer::ui::menu
{

struct ApplicationMenuHandlers
{
    std::function<void()> on_new_project;
    std::function<void()> on_open_project;
    std::function<void()> on_save_project;
    std::function<void()> on_save_project_as;
    std::function<void()> on_close_project;
    std::function<void()> on_exit_application;

    std::function<void()> on_undo;
    std::function<void()> on_redo;
    std::function<void()> on_cut;
    std::function<void()> on_copy;
    std::function<void()> on_paste;
    std::function<void()> on_delete_selection;

    std::function<void()> on_toggle_project_explorer;
    std::function<void()> on_toggle_workspace;
    std::function<void()> on_toggle_inspector;
    std::function<void()> on_toggle_status_bar;
    std::function<void()> on_reset_layout;

    std::function<void()> on_new_schema;
    std::function<void()> on_new_entity;
    std::function<void()> on_new_relationship;
    std::function<void()> on_new_map;
    std::function<void()> on_import_asset;

    std::function<void()> on_validate_project;
    std::function<void()> on_open_diagnostics;
    std::function<void()> on_open_search;

    std::function<void()> on_export_universal;
    std::function<void()> on_export_unreal;
    std::function<void()> on_export_unity;
    std::function<void()> on_export_godot;

    std::function<void()> on_show_about;
    std::function<void()> on_show_documentation;
};

struct ApplicationMenuState
{
    bool has_project          = false;
    bool has_selection        = false;
    bool has_clipboard        = false;
    bool show_project_explorer = true;
    bool show_workspace       = true;
    bool show_inspector       = true;
    bool show_status_bar      = true;
};

class ApplicationMenuBuilder
{
public:
    ApplicationMenuBuilder() = delete;

    static void Build(MenuBar& menu_bar,
                      const ApplicationMenuState& state,
                      const ApplicationMenuHandlers& handlers);

private:
    static void BuildFileMenu(MenuBar& menu_bar,
                              const ApplicationMenuState& state,
                              const ApplicationMenuHandlers& handlers);

    static void BuildEditMenu(MenuBar& menu_bar,
                              const ApplicationMenuState& state,
                              const ApplicationMenuHandlers& handlers);

    static void BuildViewMenu(MenuBar& menu_bar,
                              const ApplicationMenuState& state,
                              const ApplicationMenuHandlers& handlers);

    static void BuildProjectMenu(MenuBar& menu_bar,
                                 const ApplicationMenuState& state,
                                 const ApplicationMenuHandlers& handlers);

    static void BuildToolsMenu(MenuBar& menu_bar,
                               const ApplicationMenuState& state,
                               const ApplicationMenuHandlers& handlers);

    static void BuildExportMenu(MenuBar& menu_bar,
                                const ApplicationMenuState& state,
                                const ApplicationMenuHandlers& handlers);

    static void BuildHelpMenu(MenuBar& menu_bar,
                              const ApplicationMenuState& state,
                              const ApplicationMenuHandlers& handlers);
};

} // namespace ellindyer::ui::menu
