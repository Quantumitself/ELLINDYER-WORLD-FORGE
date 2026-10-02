#include "UI/Toolbar/ApplicationToolbarBuilder.hpp"

#include <utility>

namespace ellindyer::ui::toolbar
{

void ApplicationToolbarBuilder::Build(Toolbar& toolbar,
                                      const ApplicationToolbarState& state,
                                      const ApplicationToolbarHandlers& handlers)
{
    toolbar.Clear();

    ToolbarItem new_project_item =
        MakeButton("New", "toolbar.new_project", handlers.on_new_project,
                   "Create a new project");
    SetToolbarItemEnabled(new_project_item, true);
    toolbar.AddItem(std::move(new_project_item));

    ToolbarItem open_project_item =
        MakeButton("Open", "toolbar.open_project", handlers.on_open_project,
                   "Open an existing project");
    toolbar.AddItem(std::move(open_project_item));

    ToolbarItem save_project_item =
        MakeButton("Save", "toolbar.save_project", handlers.on_save_project,
                   "Save the current project");
    SetToolbarItemEnabled(save_project_item, state.has_project);
    toolbar.AddItem(std::move(save_project_item));

    toolbar.AddSeparator();

    ToolbarItem undo_item =
        MakeButton("Undo", "toolbar.undo", handlers.on_undo,
                   "Undo the last action");
    SetToolbarItemEnabled(undo_item, state.can_undo);
    toolbar.AddItem(std::move(undo_item));

    ToolbarItem redo_item =
        MakeButton("Redo", "toolbar.redo", handlers.on_redo,
                   "Redo the last undone action");
    SetToolbarItemEnabled(redo_item, state.can_redo);
    toolbar.AddItem(std::move(redo_item));

    toolbar.AddSeparator();

    ToolbarItem toggle_explorer_item =
        MakeToggle("Explorer", "toolbar.toggle_project_explorer",
                   state.show_project_explorer,
                   handlers.on_toggle_project_explorer,
                   "Show or hide the Project Explorer");
    toolbar.AddItem(std::move(toggle_explorer_item));

    ToolbarItem toggle_inspector_item =
        MakeToggle("Inspector", "toolbar.toggle_inspector",
                   state.show_inspector,
                   handlers.on_toggle_inspector,
                   "Show or hide the Inspector");
    toolbar.AddItem(std::move(toggle_inspector_item));

    toolbar.AddSeparator();

    ToolbarItem validate_item =
        MakeButton("Validate", "toolbar.validate_project", handlers.on_validate_project,
                   "Validate the current project");
    SetToolbarItemEnabled(validate_item, state.has_project);
    toolbar.AddItem(std::move(validate_item));

    ToolbarItem search_item =
        MakeButton("Search", "toolbar.open_search", handlers.on_open_search,
                   "Open global search");
    toolbar.AddItem(std::move(search_item));

    toolbar.AddSpacer(12.0f);

    ToolbarItem export_item =
        MakeButton("Export", "toolbar.export_universal", handlers.on_export_universal,
                   "Run universal export");
    SetToolbarItemEnabled(export_item, state.has_project);
    toolbar.AddItem(std::move(export_item));
}

} // namespace ellindyer::ui::toolbar
