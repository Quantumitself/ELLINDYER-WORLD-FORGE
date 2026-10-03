#include "UI/Dialogs/NewProjectDialog.hpp"

#include <algorithm>
#include <cstring>
#include <utility>

#include <imgui.h>

#include "Core/ErrorCode.hpp"
#include "Core/PathHelpers.hpp"
#include "UI/Dialogs/PlatformFileDialog.hpp"
#include "UI/ImGui/ImGuiCompat.hpp"

namespace ellindyer::ui::dialogs
{

namespace
{

constexpr std::size_t kNameBufferSize        = sizeof(NewProjectDialog::name_buffer_);
constexpr std::size_t kAuthorBufferSize      = sizeof(NewProjectDialog::author_buffer_);
constexpr std::size_t kOrganizationBufferSize = sizeof(NewProjectDialog::organization_buffer_);
constexpr std::size_t kWorldNameBufferSize   = sizeof(NewProjectDialog::world_name_buffer_);
constexpr std::size_t kVersionBufferSize     = sizeof(NewProjectDialog::version_buffer_);
constexpr std::size_t kEngineBufferSize      = sizeof(NewProjectDialog::engine_buffer_);
constexpr std::size_t kDescriptionBufferSize = sizeof(NewProjectDialog::description_buffer_);
constexpr std::size_t kWorldDescriptionBufferSize = sizeof(NewProjectDialog::world_description_buffer_);
constexpr std::size_t kCustomEngineBufferSize = sizeof(NewProjectDialog::custom_engine_buffer_);

const char* kEngineOptions[] = {
    "Unreal", "Unity", "Godot", "Universal", "Custom"
};
constexpr int kEngineOptionCount = IM_ARRAYSIZE(kEngineOptions);
constexpr int kCustomEngineIndex = kEngineOptionCount - 1;

void CopyText(char* buffer, std::size_t buffer_size, const std::string& text)
{
    if (buffer_size == 0)
    {
        return;
    }
    const std::size_t copy_length = std::min(buffer_size - 1, text.size());
    std::memcpy(buffer, text.data(), copy_length);
    buffer[copy_length] = '\0';
}

std::string Trim(const std::string& text)
{
    std::size_t start = 0;
    std::size_t end = text.size();
    while (start < end && (text[start] == ' ' || text[start] == '\t'))
    {
        ++start;
    }
    while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t'))
    {
        --end;
    }
    return text.substr(start, end - start);
}

int IndexOfEngine(const std::string& engine)
{
    for (int i = 0; i < kEngineOptionCount; ++i)
    {
        if (engine == kEngineOptions[i])
        {
            return i;
        }
    }
    return kCustomEngineIndex;
}

} // namespace

NewProjectDialog::NewProjectDialog()
    : DialogBase("##NewProjectDialog", "New Project")
{
    SetSubtitle("Create a new Ellindyer World Forge project.");

    DialogBaseStyle style{};
    style.width  = 620.0f;
    style.height = 620.0f;
    SetStyle(style);

    root_picker_.SetLabel("Project Root");
    root_picker_.SetHint(
        "Choose an empty folder. The project will be created inside it.");
    PathPickerStyle picker_style{};
    picker_style.browse_button_width = 78.0f;
    root_picker_.SetStyle(picker_style);

    root_picker_.SetBrowseHandler([this]()
    {
        const std::filesystem::path initial = root_picker_.GetValue();
        const std::filesystem::path picked =
            PlatformFileDialog::PickFolder(initial);
        if (!picked.empty())
        {
            root_picker_.SetValue(picked);
        }
    });
}

NewProjectDialog::~NewProjectDialog() = default;

void NewProjectDialog::SetDefaults(const NewProjectDefaultOptions& defaults)
{
    defaults_ = defaults;
}

void NewProjectDialog::SetAcceptHandler(AcceptHandler handler)
{
    accept_handler_ = std::move(handler);
}

const NewProjectRequest& NewProjectDialog::GetRequest() const noexcept
{
    return request_;
}

void NewProjectDialog::OnOpened()
{
    LoadDefaultsIfEmpty();

    // Set combo index from current engine_buffer_
    const std::string engine = Trim(std::string(engine_buffer_));
    engine_combo_index_ = IndexOfEngine(engine);

    if (engine_combo_index_ == kCustomEngineIndex)
    {
        CopyText(custom_engine_buffer_, kCustomEngineBufferSize, engine);
    }
}

void NewProjectDialog::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    ellindyer::ui::imgui_compat::PushFont(fonts.default_regular);

    const bool browse_pressed = root_picker_.Render(fonts);
    (void)browse_pressed;

    (void)browse_pressed;

    (void)fonts;

    ImGui::Spacing();

    ImGui::TextUnformatted("Project Name");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##NewProjectName", name_buffer_, kNameBufferSize);

    ImGui::Spacing();

    ImGui::TextUnformatted("Author");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##NewProjectAuthor", author_buffer_, kAuthorBufferSize);

    ImGui::Spacing();

    ImGui::TextUnformatted("Organization");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##NewProjectOrg", organization_buffer_, kOrganizationBufferSize);

    ImGui::Spacing();

    ImGui::TextUnformatted("Project Version");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##NewProjectVersion", version_buffer_, kVersionBufferSize);

    ImGui::Spacing();

    ImGui::TextUnformatted("Target Engine");
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::BeginCombo("##NewProjectEngineCombo", kEngineOptions[engine_combo_index_]))
    {
        for (int i = 0; i < kEngineOptionCount; ++i)
        {
            const bool selected = (engine_combo_index_ == i);
            if (ImGui::Selectable(kEngineOptions[i], selected))
            {
                engine_combo_index_ = i;
                if (i != kCustomEngineIndex)
                {
                    CopyText(engine_buffer_, kEngineBufferSize, kEngineOptions[i]);
                }
                else if (custom_engine_buffer_[0] != '\0')
                {
                    CopyText(engine_buffer_, kEngineBufferSize, custom_engine_buffer_);
                }
            }
            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (engine_combo_index_ == kCustomEngineIndex)
    {
        ImGui::TextUnformatted("Custom Engine Name");
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputText("##NewProjectCustomEngine",
                             custom_engine_buffer_,
                             kCustomEngineBufferSize))
        {
            CopyText(engine_buffer_, kEngineBufferSize, custom_engine_buffer_);
        }
    }

    ImGui::Spacing();

    ImGui::TextUnformatted("World Name");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##NewProjectWorldName", world_name_buffer_, kWorldNameBufferSize);

    ImGui::Spacing();

    ImGui::TextUnformatted("Description");
    ImGui::InputTextMultiline("##NewProjectDescription",
                              description_buffer_,
                              kDescriptionBufferSize,
                              ImVec2(-1.0f, 60.0f));

    ImGui::Spacing();

    ImGui::TextUnformatted("World Description");
    ImGui::InputTextMultiline("##NewProjectWorldDescription",
                              world_description_buffer_,
                              kWorldDescriptionBufferSize,
                              ImVec2(-1.0f, 60.0f));

    ImGui::PopFont();
}

bool NewProjectDialog::CanAccept() const
{
    return ValidateInput().HasValue();
}

void NewProjectDialog::OnAccept()
{
    request_.root                 = root_picker_.GetValue();
    request_.name                 = Trim(std::string(name_buffer_));
    request_.author               = Trim(std::string(author_buffer_));
    request_.organization         = Trim(std::string(organization_buffer_));
    request_.description          = Trim(std::string(description_buffer_));
    request_.world_name           = Trim(std::string(world_name_buffer_));
    request_.world_description    = Trim(std::string(world_description_buffer_));
    request_.project_version      = Trim(std::string(version_buffer_));
    request_.engine_target        = Trim(std::string(engine_buffer_));

    if (accept_handler_)
    {
        accept_handler_(request_);
    }
}

ellindyer::core::Result<void> NewProjectDialog::ValidateInput() const
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    const std::filesystem::path root = root_picker_.GetValue();
    if (root.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project root is required."));
    }

    const std::string name = Trim(std::string(name_buffer_));
    if (name.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project name is required."));
    }

    if (name.size() > 128)
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project name must be 128 characters or fewer."));
    }

    const std::string engine = Trim(std::string(engine_buffer_));
    if (engine.size() > 64)
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Target engine must be 64 characters or fewer."));
    }

    const std::string version = Trim(std::string(version_buffer_));
    if (version.size() > 64)
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project version must be 64 characters or fewer."));
    }

    return Result<void>{};
}

void NewProjectDialog::LoadDefaultsIfEmpty()
{
    if (root_picker_.GetValue().empty() && !defaults_.suggested_root.empty())
    {
        root_picker_.SetValue(defaults_.suggested_root);
    }

    if (name_buffer_[0] == '\0' && !defaults_.suggested_root.empty())
    {
        CopyText(name_buffer_, kNameBufferSize,
                 defaults_.suggested_root.filename().string());
    }

    if (author_buffer_[0] == '\0' && !defaults_.author.empty())
    {
        CopyText(author_buffer_, kAuthorBufferSize, defaults_.author);
    }

    if (organization_buffer_[0] == '\0' && !defaults_.organization.empty())
    {
        CopyText(organization_buffer_, kOrganizationBufferSize, defaults_.organization);
    }

    if (world_name_buffer_[0] == '\0' && name_buffer_[0] != '\0')
    {
        CopyText(world_name_buffer_, kWorldNameBufferSize, std::string(name_buffer_));
    }

    if (version_buffer_[0] == '\0' && !defaults_.default_project_version.empty())
    {
        CopyText(version_buffer_, kVersionBufferSize, defaults_.default_project_version);
    }

    if (engine_buffer_[0] == '\0' && !defaults_.default_engine_target.empty())
    {
        CopyText(engine_buffer_, kEngineBufferSize, defaults_.default_engine_target);
    }
}

void NewProjectDialog::UpdateEngineBufferFromSelection()
{
    if (engine_combo_index_ < 0 || engine_combo_index_ >= kEngineOptionCount)
    {
        return;
    }

    if (engine_combo_index_ == kCustomEngineIndex)
    {
        CopyText(engine_buffer_, kEngineBufferSize, custom_engine_buffer_);
    }
    else
    {
        CopyText(engine_buffer_, kEngineBufferSize, kEngineOptions[engine_combo_index_]);
    }
}

} // namespace ellindyer::ui::dialogs
