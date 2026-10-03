#include "UI/Dialogs/OpenProjectDialog.hpp"
#include "UI/Dialogs/PlatformFileDialog.hpp"

#include <algorithm>
#include <utility>

#include <imgui.h>

#include "Core/ErrorCode.hpp"

namespace ellindyer::ui::dialogs
{

namespace
{

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

} // namespace

OpenProjectDialog::OpenProjectDialog()
    : DialogBase("##OpenProjectDialog", "Open Project")
{
    SetSubtitle("Open an existing Ellindyer World Forge project.");

    DialogBaseStyle style{};
    style.width  = 620.0f;
    style.height = 520.0f;
    SetStyle(style);

    root_picker_.SetLabel("Project Root");
    root_picker_.SetHint(
        "Choose a project folder containing a Project.wforge manifest.");
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

    RecentProjectsListStyle list_style{};
    recent_list_.SetStyle(list_style);
}

OpenProjectDialog::~OpenProjectDialog() = default;

void OpenProjectDialog::SetDefaults(const OpenProjectDefaultOptions& defaults)
{
    defaults_ = defaults;
}

void OpenProjectDialog::SetAcceptHandler(AcceptHandler handler)
{
    accept_handler_ = std::move(handler);
}

const OpenProjectRequest& OpenProjectDialog::GetRequest() const noexcept
{
    return request_;
}

void OpenProjectDialog::OnOpened()
{
    recent_list_.SetEntries(defaults_.recent_projects);

    if (!defaults_.suggested_root.empty())
    {
        root_picker_.SetValue(defaults_.suggested_root);
        recent_list_.SetSelectedIndex(-1);
        recent_has_priority_ = false;
    }
    else if (recent_list_.HasEntries())
    {
        root_picker_.SetValue(recent_list_.GetSelectedPath());
        recent_has_priority_ = true;
    }
    else
    {
        root_picker_.SetValue({});
        recent_has_priority_ = false;
    }
}

void OpenProjectDialog::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    if (fonts.small_regular != nullptr)
    {
        ImGui::PushFont(fonts.small_regular, fonts.small_regular->LegacySize);
    }
    ImGui::TextUnformatted("Recent Projects");
    if (fonts.small_regular != nullptr)
    {
        ImGui::PopFont();
    }

    ImGui::Spacing();

    bool double_clicked = false;
    const bool selection_changed =
        recent_list_.Render(fonts, 160.0f, &double_clicked);

    if (selection_changed)
    {
        if (recent_list_.HasSelection())
        {
            root_picker_.SetValue(recent_list_.GetSelectedPath());
            recent_has_priority_ = true;
        }
    }

    if (double_clicked && recent_list_.HasSelection())
    {
        root_picker_.SetValue(recent_list_.GetSelectedPath());
        recent_has_priority_ = true;
        SyncRequestFromControls();
        if (CanAccept())
        {
            OnAccept();
            Close();
            if (fonts.default_regular != nullptr)
            {
                ImGui::PopFont();
            }
            return;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (root_picker_.Render(fonts))
    {
        // No native file dialog is available; the text field remains the
        // canonical input. The Browse button is present for consistency with
        // the New Project dialog.
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

bool OpenProjectDialog::CanAccept() const
{
    return ValidateInput().HasValue();
}

void OpenProjectDialog::OnAccept()
{
    SyncRequestFromControls();

    if (accept_handler_)
    {
        accept_handler_(request_);
    }
}

ellindyer::core::Result<void> OpenProjectDialog::ValidateInput() const
{
    using ellindyer::core::ErrorCode;
    using ellindyer::core::MakeError;
    using ellindyer::core::Result;

    std::filesystem::path candidate = root_picker_.GetValue();
    if (candidate.empty() && recent_list_.HasSelection())
    {
        candidate = recent_list_.GetSelectedPath();
    }

    if (candidate.empty())
    {
        return Result<void>(MakeError(ErrorCode::InvalidArgument,
                                      "Project root is required."));
    }

    return Result<void>{};
}

void OpenProjectDialog::SyncRequestFromControls()
{
    std::filesystem::path candidate = root_picker_.GetValue();
    if (candidate.empty() && recent_list_.HasSelection())
    {
        candidate = recent_list_.GetSelectedPath();
    }

    request_.root = candidate;
    request_.require_manifest = true;
}

} // namespace ellindyer::ui::dialogs
