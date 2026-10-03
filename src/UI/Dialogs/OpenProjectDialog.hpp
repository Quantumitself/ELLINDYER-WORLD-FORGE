#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Dialogs/DialogBase.hpp"
#include "UI/Dialogs/PathPicker.hpp"
#include "UI/Dialogs/RecentProjectsList.hpp"

namespace ellindyer::ui::dialogs
{

struct OpenProjectRequest
{
    std::filesystem::path root;
    bool                  require_manifest = true;
};

struct OpenProjectDefaultOptions
{
    std::vector<std::filesystem::path> recent_projects;
    std::filesystem::path              suggested_root;
};

class OpenProjectDialog : public DialogBase
{
public:
    using AcceptHandler = std::function<void(const OpenProjectRequest&)>;

    OpenProjectDialog();
    ~OpenProjectDialog() override;

    OpenProjectDialog(const OpenProjectDialog&) = delete;
    OpenProjectDialog& operator=(const OpenProjectDialog&) = delete;
    OpenProjectDialog(OpenProjectDialog&&) noexcept = delete;
    OpenProjectDialog& operator=(OpenProjectDialog&&) noexcept = delete;

    void SetDefaults(const OpenProjectDefaultOptions& defaults);

    void SetAcceptHandler(AcceptHandler handler);

    [[nodiscard]] const OpenProjectRequest& GetRequest() const noexcept;

protected:
    void OnOpened() override;

    void RenderBody(const ellindyer::ui::fonts::FontSet& fonts) override;

    [[nodiscard]] bool CanAccept() const override;

    void OnAccept() override;

private:
    [[nodiscard]] ellindyer::core::Result<void> ValidateInput() const;

    void SyncRequestFromControls();

    OpenProjectDefaultOptions  defaults_{};
    OpenProjectRequest         request_{};
    PathPicker                 root_picker_{};
    RecentProjectsList         recent_list_{};
    AcceptHandler              accept_handler_;
    bool                       recent_has_priority_ = false;
};

} // namespace ellindyer::ui::dialogs
