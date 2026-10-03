#include "UI/Dialogs/DialogHost.hpp"

#include <algorithm>
#include <utility>

namespace ellindyer::ui::dialogs
{

DialogHost::DialogHost() = default;

DialogHost::~DialogHost() = default;

void DialogHost::RegisterDialog(std::shared_ptr<DialogBase> dialog)
{
    if (!dialog)
    {
        return;
    }
    dialogs_.push_back(std::move(dialog));
}

void DialogHost::UnregisterDialog(const std::string& identifier)
{
    if (identifier.empty())
    {
        return;
    }
    dialogs_.erase(
        std::remove_if(dialogs_.begin(),
                       dialogs_.end(),
                       [&identifier](const std::shared_ptr<DialogBase>& dialog)
                       {
                           return dialog && dialog->GetIdentifier() == identifier;
                       }),
        dialogs_.end());
}

void DialogHost::Clear()
{
    dialogs_.clear();
}

std::shared_ptr<DialogBase> DialogHost::FindDialog(const std::string& identifier)
{
    for (const std::shared_ptr<DialogBase>& dialog : dialogs_)
    {
        if (dialog && dialog->GetIdentifier() == identifier)
        {
            return dialog;
        }
    }
    return nullptr;
}

bool DialogHost::HasOpenDialog() const noexcept
{
    for (const std::shared_ptr<DialogBase>& dialog : dialogs_)
    {
        if (dialog && dialog->IsOpen())
        {
            return true;
        }
    }
    return false;
}

void DialogHost::Render(const ellindyer::ui::fonts::FontSet& fonts)
{
    for (const std::shared_ptr<DialogBase>& dialog : dialogs_)
    {
        if (!dialog)
        {
            continue;
        }
        if (!dialog->IsOpen())
        {
            continue;
        }
        const ellindyer::ui::dialogs::DialogResult dialog_result = dialog->Render(fonts);
        (void)dialog_result;
    }
}

} // namespace ellindyer::ui::dialogs
