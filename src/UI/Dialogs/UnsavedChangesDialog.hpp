#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "UI/Dialogs/DialogBase.hpp"

namespace ellindyer::ui::dialogs
{

enum class UnsavedChangesChoice : std::uint8_t
{
    None,
    Save,
    Discard,
    Cancel
};

struct UnsavedChangesPromptOptions
{
    std::string project_name;
    std::string action_description = "continue";
};

class UnsavedChangesDialog : public DialogBase
{
public:
    using ChoiceHandler = std::function<void(UnsavedChangesChoice)>;

    UnsavedChangesDialog();
    ~UnsavedChangesDialog() override;

    UnsavedChangesDialog(const UnsavedChangesDialog&) = delete;
    UnsavedChangesDialog& operator=(const UnsavedChangesDialog&) = delete;
    UnsavedChangesDialog(UnsavedChangesDialog&&) noexcept = delete;
    UnsavedChangesDialog& operator=(UnsavedChangesDialog&&) noexcept = delete;

    void SetPromptOptions(const UnsavedChangesPromptOptions& options);

    void SetChoiceHandler(ChoiceHandler handler);

    [[nodiscard]] UnsavedChangesChoice GetLastChoice() const noexcept;

protected:
    void OnOpened() override;

    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts) override;

    [[nodiscard]] bool CanAccept() const override;

    [[nodiscard]] bool CanCancel() const override;

    void OnAccept() override;

    void OnCancel() override;

    [[nodiscard]] const char* GetAcceptLabel() const override;

    [[nodiscard]] const char* GetCancelLabel() const override;

private:
    UnsavedChangesPromptOptions prompt_{};
    ChoiceHandler               choice_handler_;
    UnsavedChangesChoice        last_choice_ = UnsavedChangesChoice::None;
    int                         selected_action_ = 0;
};

} // namespace ellindyer::ui::dialogs
