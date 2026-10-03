#pragma once

#include <memory>
#include <vector>

#include "UI/Dialogs/DialogBase.hpp"
#include "UI/Fonts/FontManager.hpp"

namespace ellindyer::ui::dialogs
{

class DialogHost
{
public:
    DialogHost();
    ~DialogHost();

    DialogHost(const DialogHost&) = delete;
    DialogHost& operator=(const DialogHost&) = delete;
    DialogHost(DialogHost&&) noexcept = delete;
    DialogHost& operator=(DialogHost&&) noexcept = delete;

    void RegisterDialog(std::shared_ptr<DialogBase> dialog);

    void UnregisterDialog(const std::string& identifier);

    void Clear();

    [[nodiscard]] std::shared_ptr<DialogBase> FindDialog(const std::string& identifier);

    [[nodiscard]] bool HasOpenDialog() const noexcept;

    void Render(const ellindyer::ui::fonts::FontSet& fonts);

private:
    std::vector<std::shared_ptr<DialogBase>> dialogs_;
};

} // namespace ellindyer::ui::dialogs
