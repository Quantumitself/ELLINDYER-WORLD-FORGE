#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Dialogs/DialogBase.hpp"
#include "UI/Dialogs/PathPicker.hpp"

namespace ellindyer::ui::dialogs
{

struct SaveProjectAsRequest
{
    std::filesystem::path target_root;
    bool                  overwrite_existing = false;
};

struct SaveProjectAsDefaultOptions
{
    std::filesystem::path current_root;
    std::filesystem::path suggested_target;
    std::string           project_name;
};

class SaveProjectAsDialog : public DialogBase
{
public:
    using AcceptHandler = std::function<void(const SaveProjectAsRequest&)>;

    SaveProjectAsDialog();
    ~SaveProjectAsDialog() override;

    SaveProjectAsDialog(const SaveProjectAsDialog&) = delete;
    SaveProjectAsDialog& operator=(const SaveProjectAsDialog&) = delete;
    SaveProjectAsDialog(SaveProjectAsDialog&&) noexcept = delete;
    SaveProjectAsDialog& operator=(SaveProjectAsDialog&&) noexcept = delete;

    void SetDefaults(const SaveProjectAsDefaultOptions& defaults);

    void SetAcceptHandler(AcceptHandler handler);

    [[nodiscard]] const SaveProjectAsRequest& GetRequest() const noexcept;

protected:
    void OnOpened() override;

    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts) override;

    [[nodiscard]] bool CanAccept() const override;

    void OnAccept() override;

private:
    [[nodiscard]] ellindyer::core::Result<void> ValidateInput() const;

    void SyncRequestFromControls();

    SaveProjectAsDefaultOptions defaults_{};
    SaveProjectAsRequest       request_{};
    PathPicker                 target_picker_{};
    AcceptHandler              accept_handler_;
    bool                       overwrite_existing_ = false;
};

} // namespace ellindyer::ui::dialogs
