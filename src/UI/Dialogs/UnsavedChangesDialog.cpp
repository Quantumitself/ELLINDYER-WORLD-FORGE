#include "UI/Dialogs/UnsavedChangesDialog.hpp"

#include <utility>

#include <imgui.h>

namespace ellindyer::ui::dialogs
{

namespace
{

constexpr const char* kSaveAndContinueLabel = "Save and Continue";
constexpr const char* kDiscardLabel         = "Discard Changes";
constexpr const char* kCancelLabel          = "Cancel";

} // namespace

UnsavedChangesDialog::UnsavedChangesDialog()
    : DialogBase("##UnsavedChangesDialog", "Unsaved Changes")
{
    SetSubtitle("The current project has unsaved changes.");

    DialogBaseStyle style{};
    style.width  = 520.0f;
    style.height = 240.0f;
    SetStyle(style);
}

UnsavedChangesDialog::~UnsavedChangesDialog() = default;

void UnsavedChangesDialog::SetPromptOptions(const UnsavedChangesPromptOptions& options)
{
    prompt_ = options;
}

void UnsavedChangesDialog::SetChoiceHandler(ChoiceHandler handler)
{
    choice_handler_ = std::move(handler);
}

UnsavedChangesChoice UnsavedChangesDialog::GetLastChoice() const noexcept
{
    return last_choice_;
}

void UnsavedChangesDialog::OnOpened()
{
    selected_action_ = 0;
    last_choice_ = UnsavedChangesChoice::None;
}

void UnsavedChangesDialog::RenderBody(const ellindyer::ui::fonts::FontSet& fonts)
{
    if (fonts.default_regular != nullptr)
    {
        ImGui::PushFont(fonts.default_regular, fonts.default_regular->LegacySize);
    }

    const std::string project_name = prompt_.project_name.empty()
        ? std::string{"the current project"}
        : prompt_.project_name;

    ImGui::TextWrapped(
        "\"%s\" has unsaved changes. Choose an action before you %s.",
        project_name.c_str(),
        prompt_.action_description.c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::RadioButton("Save and Continue", selected_action_ == 0))
    {
        selected_action_ = 0;
    }
    if (ImGui::RadioButton("Discard Changes", selected_action_ == 1))
    {
        selected_action_ = 1;
    }
    if (ImGui::RadioButton("Cancel", selected_action_ == 2))
    {
        selected_action_ = 2;
    }

    if (fonts.default_regular != nullptr)
    {
        ImGui::PopFont();
    }
}

bool UnsavedChangesDialog::CanAccept() const
{
    return true;
}

bool UnsavedChangesDialog::CanCancel() const
{
    return true;
}

void UnsavedChangesDialog::OnAccept()
{
    UnsavedChangesChoice choice = UnsavedChangesChoice::Cancel;

    switch (selected_action_)
    {
    case 0: choice = UnsavedChangesChoice::Save;    break;
    case 1: choice = UnsavedChangesChoice::Discard; break;
    case 2: choice = UnsavedChangesChoice::Cancel;  break;
    default: break;
    }

    last_choice_ = choice;

    if (choice_handler_)
    {
        choice_handler_(choice);
    }
}

void UnsavedChangesDialog::OnCancel()
{
    last_choice_ = UnsavedChangesChoice::Cancel;

    if (choice_handler_)
    {
        choice_handler_(UnsavedChangesChoice::Cancel);
    }
}

const char* UnsavedChangesDialog::GetAcceptLabel() const
{
    return "Continue";
}

const char* UnsavedChangesDialog::GetCancelLabel() const
{
    return kCancelLabel;
}

} // namespace ellindyer::ui::dialogs
