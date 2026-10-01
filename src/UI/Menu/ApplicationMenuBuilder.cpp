#include "UI/Menu/ApplicationMenuBuilder.hpp"

#include <utility>

namespace ellindyer::ui::menu
{

namespace
{

MenuItem Command(const char* label,
                 const char* identifier,
                 std::function<void()> action,
                 const char* shortcut,
                 bool enabled)
{
    MenuItem item = MakeCommand(label, identifier, std::move(action),
                                shortcut != nullptr ? std::string(shortcut) : std::string{});
    item.enabled = enabled;
    return item;
}

MenuItem CheckCommand(const char* label,
                      const char* identifier,
                      std::function<void()> action,
                      bool checked)
{
    MenuItem item = MakeCommand(label, identifier, std::move(action));
    item.has_check = true;
    item.checked   = checked;
    return item;
}

} // namespace

void ApplicationMenuBuilder::Build(MenuBar& menu_bar,
                                   const ApplicationMenuState& state,
                                   const ApplicationMenuHandlers& handlers)
{
    menu_bar.Clear();
    BuildFileMenu(menu_bar, state, handlers);
    BuildEditMenu(menu_bar, state, handlers);
    BuildViewMenu(menu_bar, state, handlers);
    BuildProjectMenu(menu_bar, state, handlers);
    BuildToolsMenu(menu_bar, state, handlers);
    BuildExportMenu(menu_bar, state, handlers);
    BuildHelpMenu(menu_bar, state, handlers);
}

void ApplicationMenuBuilder::BuildFileMenu(MenuBar& menu_bar,
                                           const ApplicationMenuState& state,
                                           const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(Command("New Project...", "file.new_project",
                            handlers.on_new_project, "Ctrl+N", true));
    items.push_back(Command("Open Project...", "file.open_project",
                            handlers.on_open_project, "Ctrl+O", true));
    items.push_back(MakeSeparator());
    items.push_back(Command("Save Project", "file.save_project",
                            handlers.on_save_project, "Ctrl+S", state.has_project));
    items.push_back(Command("Save Project As...", "file.save_project_as",
                            handlers.on_save_project_as, "Ctrl+Shift+S", state.has_project));
    items.push_back(Command("Close Project", "file.close_project",
                            handlers.on_close_project, "", state.has_project));
    items.push_back(MakeSeparator());
    items.push_back(Command("Exit", "file.exit",
                            handlers.on_exit_application, "Alt+F4", true));

    menu_bar.AddMenu("File", std::move(items));
}

void ApplicationMenuBuilder::BuildEditMenu(MenuBar& menu_bar,
                                           const ApplicationMenuState& state,
                                           const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(Command("Undo", "edit.undo",
                            handlers.on_undo, "Ctrl+Z", state.has_project));
    items.push_back(Command("Redo", "edit.redo",
                            handlers.on_redo, "Ctrl+Y", state.has_project));
    items.push_back(MakeSeparator());
    items.push_back(Command("Cut", "edit.cut",
                            handlers.on_cut, "Ctrl+X", state.has_selection));
    items.push_back(Command("Copy", "edit.copy",
                            handlers.on_copy, "Ctrl+C", state.has_selection));
    items.push_back(Command("Paste", "edit.paste",
                            handlers.on_paste, "Ctrl+V", state.has_clipboard));
    items.push_back(Command("Delete", "edit.delete",
                            handlers.on_delete_selection, "Del", state.has_selection));

    menu_bar.AddMenu("Edit", std::move(items));
}

void ApplicationMenuBuilder::BuildViewMenu(MenuBar& menu_bar,
                                           const ApplicationMenuState& state,
                                           const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(CheckCommand("Project Explorer", "view.toggle_project_explorer",
                                 handlers.on_toggle_project_explorer,
                                 state.show_project_explorer));
    items.push_back(CheckCommand("Workspace", "view.toggle_workspace",
                                 handlers.on_toggle_workspace,
                                 state.show_workspace));
    items.push_back(CheckCommand("Inspector", "view.toggle_inspector",
                                 handlers.on_toggle_inspector,
                                 state.show_inspector));
    items.push_back(CheckCommand("Status Bar", "view.toggle_status_bar",
                                 handlers.on_toggle_status_bar,
                                 state.show_status_bar));
    items.push_back(MakeSeparator());
    items.push_back(Command("Reset Layout", "view.reset_layout",
                            handlers.on_reset_layout, "", true));

    menu_bar.AddMenu("View", std::move(items));
}

void ApplicationMenuBuilder::BuildProjectMenu(MenuBar& menu_bar,
                                              const ApplicationMenuState& state,
                                              const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(Command("New Schema...", "project.new_schema",
                            handlers.on_new_schema, "", state.has_project));
    items.push_back(Command("New Entity...", "project.new_entity",
                            handlers.on_new_entity, "", state.has_project));
    items.push_back(Command("New Relationship...", "project.new_relationship",
                            handlers.on_new_relationship, "", state.has_project));
    items.push_back(MakeSeparator());
    items.push_back(Command("New Map...", "project.new_map",
                            handlers.on_new_map, "", state.has_project));
    items.push_back(Command("Import Asset...", "project.import_asset",
                            handlers.on_import_asset, "", state.has_project));

    menu_bar.AddMenu("Project", std::move(items));
}

void ApplicationMenuBuilder::BuildToolsMenu(MenuBar& menu_bar,
                                            const ApplicationMenuState& state,
                                            const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(Command("Validate Project", "tools.validate_project",
                            handlers.on_validate_project, "F7", state.has_project));
    items.push_back(Command("Diagnostics", "tools.open_diagnostics",
                            handlers.on_open_diagnostics, "", true));
    items.push_back(Command("Search...", "tools.open_search",
                            handlers.on_open_search, "Ctrl+P", true));

    menu_bar.AddMenu("Tools", std::move(items));
}

void ApplicationMenuBuilder::BuildExportMenu(MenuBar& menu_bar,
                                             const ApplicationMenuState& state,
                                             const ApplicationMenuHandlers& handlers)
{
    std::vector<MenuItem> items;

    items.push_back(Command("Universal Export...", "export.universal",
                            handlers.on_export_universal, "", state.has_project));
    items.push_back(MakeSeparator());
    items.push_back(Command("Unreal Export...", "export.unreal",
                            handlers.on_export_unreal, "", state.has_project));
    items.push_back(Command("Unity Export...", "export.unity",
                            handlers.on_export_unity, "", state.has_project));
    items.push_back(Command("Godot Export...", "export.godot",
                            handlers.on_export_godot, "", state.has_project));

    menu_bar.AddMenu("Export", std::move(items));
}

void ApplicationMenuBuilder::BuildHelpMenu(MenuBar& menu_bar,
                                           const ApplicationMenuState& state,
                                           const ApplicationMenuHandlers& handlers)
{
    (void)state;

    std::vector<MenuItem> items;

    items.push_back(Command("Documentation", "help.documentation",
                            handlers.on_show_documentation, "F1", true));
    items.push_back(MakeSeparator());
    items.push_back(Command("About Ellindyer World Forge", "help.about",
                            handlers.on_show_about, "", true));

    menu_bar.AddMenu("Help", std::move(items));
}

} // namespace ellindyer::ui::menu
