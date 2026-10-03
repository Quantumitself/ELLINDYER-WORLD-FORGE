#include "UI/Dialogs/SaveProjectAsDialog.hpp"

#include <system_error>
#include <utility>

#include <imgui.h>

#include "Core/ErrorCode.hpp"

namespace ellindyer::ui::dialogs
{

namespace
{

bool DirectoryExistsAndIsEmpty(const std::filesystem::path& path)
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec) || ec)
    {
        return true;
    }
    if (!std::filesystem::is_directory(path, ec) || ec)
    {
        return false;
    }
    return std::filesystem::is_empty(path, ec) && !ec;
}

} // namespace

SaveProjectAsDialog::SaveProjectAsDialog()
    : DialogBase("##SaveProjectAsDialog", "Save Project As")
{
    SetSubtitle("Save the current project to a new folder.");

    DialogBaseStyle style{};
    style.width  = 620.0f;
    style.height = 320.0f;
    SetStyle(style);

    target_picker_.SetLabel("Target Project Root");
    target_picker_.SetHint(
        "Choose an empty folder. The project will be saved into it.");
    PathPickerStyle picker_style{};
    picker_style.browse_button_width = 78.0f;
    target_picker_.SetStyle(picker_style);
}

SaveProjectAsDialog::~SaveProjectAsDialog() = default;

void SaveProjectAsDialog::SetDefaults(const SaveProjectAsDefaultOptions& defaults)
{
    defaults_ = defaults;
}

void SaveProjectAsDialog::SetAcceptHandler(AcceptHandler handler)
{
    accept_handler_ = std::move(handler);
}

const SaveProjectAsRequest& SaveProjectAsDialog::GetRequest() const noexcept
{
    return request_;
}

void SaveProjectAsDialog::OnOpened()
{
    overwrite_existing_ = false;

    if (!defaults_.suggested_target.empty())
    {
        target_picker_.SetValue(defaults_.suggested_target);
    }
    else if (!defaults_.current_root.empty())
    {
        std::filesystem::path candidate = defaults_.current_root;
        candidate += "_copy";
        target_picker_.SetValue(candidate);
    }
    else
    {
        target_picker_.SetValue({});
    }
}

void SaveProjectAsDialog::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    if (!defaults_.project_name.empty())
    {
        ImGui::TextUnformatted("Project:");
        ImGui::SameLine();
        if (fonts.default_regular != nullptr)
        {
            ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
        }
        ImGui::TextUnformatted(defaults_.project_name.c_str());
        if (fonts.default_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }

    if (!defaults_.current_root.empty())
    {
        if (fonts.small_regular != nullptr)
        {
            ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.00f));
        ImGui::Text("Current location: %s", defaults_.current_root.generic_string().c_str());
        ImGui::PopStyleColor();
        if (fonts.small_regular != nullptr)
        {
            ImGui::PopFont();
        }
    }

    ImGui::Spacing();

    if (target_picker_.Render(fonts))
    {
        // No native file dialog is available; the text field remains the
        // canonical input.
    }

    ImGui::Spacing();

    ImGui::Checkbox("Overwrite non-empty target folder",
                    &overwrite_existing_);

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

bool SaveProjectAsDialog::CanAccept() const
{
    return ValidateInput().HasValue();
}

void SaveProjectAsDialog::OnAccept()
{
    SyncRequestFromControls();

    if (accept_handler_)
    {
        accept_handler_(request_);
    }
}

ellindyer::core::Result<void> SaveProjectAsDialog::ValidateInput() const
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    const std::filesystem::path target = target_picker_.GetValue();
    if (target.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Target project root is required."));
    }

    if (!defaults_.current_root.empty())
    {
        std::error_code ec;
        const std::filesystem::path normalized_target =
            std::filesystem::absolute(target, ec).lexically_normal();
        const std::filesystem::path normalized_current =
            std::filesystem::absolute(defaults_.current_root, ec).lexically_normal();

        if (!ec && normalized_target == normalized_current)
        {
            return Result<void>(MakeError(
                ErrorCode::InvalidArgument,
                "Target folder must differ from the current project folder."));
        }
    }

    if (!overwrite_existing_ && !DirectoryExistsAndIsEmpty(target))
    {
        return Result<void>(MakeError(
            ErrorCode::AlreadyExists,
            "Target folder exists and is not empty."));
    }

    return Result<void>{};
}

void SaveProjectAsDialog::SyncRequestFromControls()
{
    request_.target_root        = target_picker_.GetValue();
    request_.overwrite_existing = overwrite_existing_;
}

} // namespace ellindyer::ui::dialogs
