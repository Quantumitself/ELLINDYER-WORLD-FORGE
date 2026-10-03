#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "Project/ProjectManager.hpp"
#include "UI/Dialogs/DialogBase.hpp"
#include "UI/Dialogs/PathPicker.hpp"

namespace ellindyer::ui::dialogs
{

struct NewProjectRequest
{
    std::filesystem::path root;
    std::string           name;
    std::string           author;
    std::string           organization;
    std::string           description;
    std::string           world_name;
    std::string           world_description;
    std::string           project_version;
    std::string           engine_target;
    bool                  overwrite_existing = false;
};

struct NewProjectDefaultOptions
{
    std::filesystem::path suggested_root;
    std::string           author;
    std::string           organization;
    std::string           default_engine_target = "Unreal";
    std::string           default_project_version = "0.1.0";
};

class NewProjectDialog : public DialogBase
{
public:
    using AcceptHandler = std::function<void(const NewProjectRequest&)>;

    NewProjectDialog();
    ~NewProjectDialog() override;

    NewProjectDialog(const NewProjectDialog&) = delete;
    NewProjectDialog& operator=(const NewProjectDialog&) = delete;
    NewProjectDialog(NewProjectDialog&&) noexcept = delete;
    NewProjectDialog& operator=(NewProjectDialog&&) noexcept = delete;

    void SetDefaults(const NewProjectDefaultOptions& defaults);
    void SetAcceptHandler(AcceptHandler handler);
    [[nodiscard]] const NewProjectRequest& GetRequest() const noexcept;

protected:
    void OnOpened() override;
    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts) override;
    [[nodiscard]] bool CanAccept() const override;
    void OnAccept() override;

private:
    [[nodiscard]] ellindyer::core::Result<void> ValidateInput() const;
    void LoadDefaultsIfEmpty();
    void UpdateEngineBufferFromSelection();

    NewProjectDefaultOptions defaults_{};
    NewProjectRequest        request_{};
    PathPicker               root_picker_{};
    AcceptHandler            accept_handler_;

    int                      engine_combo_index_ = 0;
    char                     custom_engine_buffer_[64] = {};

    char                     name_buffer_[128]     = {};
    char                     author_buffer_[128]   = {};
    char                     organization_buffer_[128] = {};
    char                     world_name_buffer_[128] = {};
    char                     version_buffer_[64]   = {};
    char                     engine_buffer_[64]    = {};
    char                     description_buffer_[1024] = {};
    char                     world_description_buffer_[1024] = {};
};

} // namespace ellindyer::ui::dialogs
