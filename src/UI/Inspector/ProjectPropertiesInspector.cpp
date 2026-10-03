#include "UI/Inspector/ProjectPropertiesInspector.hpp"

#include <imgui.h>

#include "UI/ImGui/ImGuiCompat.hpp"
#include "Project/Serialization/ProjectTimeUtils.hpp"

namespace ellindyer::ui::inspector
{

ProjectPropertiesInspector::ProjectPropertiesInspector() = default;

ProjectPropertiesInspector::~ProjectPropertiesInspector() = default;

void ProjectPropertiesInspector::SetCallbacks(
    const ProjectPropertiesInspectorCallbacks& callbacks)
{
    callbacks_ = callbacks;
}

void ProjectPropertiesInspector::Bind(ellindyer::project::Project* project)
{
    project_ = project;
}

void ProjectPropertiesInspector::Unbind()
{
    project_ = nullptr;
}

bool ProjectPropertiesInspector::IsBound() const noexcept
{
    return project_ != nullptr;
}

void ProjectPropertiesInspector::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (project_ == nullptr)
    {
        if (fonts.default_regular != nullptr)
        {
            ellindyer::ui::imgui_compat::PushFont(fonts.default_regular);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.00f));
        ImGui::TextWrapped("No project is open.");
        ImGui::TextWrapped("Create or open a project to inspect its properties.");
        ImGui::PopStyleColor();
        if (fonts.default_regular != nullptr)
        {
            ImGui::PopFont();
        }
        return;
    }

    RebuildFields();

    if (grid_.HasEdits())
    {
        if (project_ != nullptr)
        {
            project_->MarkModified();
        }
        if (callbacks_.on_modified)
        {
            callbacks_.on_modified();
        }
        grid_.ClearEditFlag();
    }

    grid_.Render(fonts);
}

void ProjectPropertiesInspector::RebuildFields()
{
    grid_.Clear();

    ellindyer::project::Project* project = project_;
    if (project == nullptr)
    {
        return;
    }

    ellindyer::project::ProjectMetadata& metadata = project->GetMutableMetadata();
    const ellindyer::project::ProjectLayout& layout = project->GetLayout();

    grid_.AddField(MakeHeading("Identity"));

    grid_.AddField(MakeReadOnlyText(
        "project.id",
        "Project ID",
        project->GetId().ToString(),
        "Stable identifier for this project."));

    grid_.AddField(MakeText(
        "project.name",
        "Project Name",
        metadata.name,
        [metadata](const std::string& new_value) mutable
        {
            metadata.name = new_value;
        },
        "Human-readable project name."));

    grid_.AddField(MakeText(
        "project.version",
        "Project Version",
        metadata.project_version,
        [metadata](const std::string& new_value) mutable
        {
            metadata.project_version = new_value;
        },
        "Version string for the project definition."));

    grid_.AddField(MakeText(
        "project.engine",
        "Target Engine",
        metadata.engine_target,
        [metadata](const std::string& new_value) mutable
        {
            metadata.engine_target = new_value;
        },
        "Engine the project is intended to be exported to."));

    grid_.AddField(MakeMultilineText(
        "project.description",
        "Description",
        metadata.description,
        [metadata](const std::string& new_value) mutable
        {
            metadata.description = new_value;
        },
        "Short project description."));

    grid_.AddField(MakeHeading("Authorship"));

    grid_.AddField(MakeText(
        "project.author",
        "Author",
        metadata.author,
        [metadata](const std::string& new_value) mutable
        {
            metadata.author = new_value;
        }));

    grid_.AddField(MakeText(
        "project.organization",
        "Organization",
        metadata.organization,
        [metadata](const std::string& new_value) mutable
        {
            metadata.organization = new_value;
        }));

    grid_.AddField(MakeHeading("World"));

    grid_.AddField(MakeText(
        "world.name",
        "World Name",
        metadata.world_name,
        [metadata](const std::string& new_value) mutable
        {
            metadata.world_name = new_value;
        }));

    grid_.AddField(MakeMultilineText(
        "world.description",
        "World Description",
        metadata.world_description,
        [metadata](const std::string& new_value) mutable
        {
            metadata.world_description = new_value;
        }));

    grid_.AddField(MakeHeading("Location"));

    grid_.AddField(MakeReadOnlyText(
        "project.root",
        "Project Root",
        layout.root.generic_string()));

    grid_.AddField(MakeReadOnlyText(
        "project.manifest",
        "Manifest",
        layout.manifest_file.generic_string()));

    grid_.AddField(MakeHeading("Timestamps"));

    grid_.AddField(MakeReadOnlyText(
        "project.created",
        "Created (UTC)",
        metadata.created_utc));

    grid_.AddField(MakeReadOnlyText(
        "project.modified",
        "Modified (UTC)",
        metadata.modified_utc));

    grid_.AddField(MakeHeading("Format"));

    grid_.AddField(MakeReadOnlyText(
        "project.formatVersion",
        "Format Version",
        std::to_string(metadata.format_version)));
}

} // namespace ellindyer::ui::inspector
