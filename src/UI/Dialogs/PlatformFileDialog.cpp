#include "UI/Dialogs/PlatformFileDialog.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>
#include <string>
#endif

namespace ellindyer::ui::dialogs
{

#if defined(_WIN32)

std::filesystem::path PlatformFileDialog::PickFolder(
    const std::filesystem::path& initial_directory)
{
    std::filesystem::path result{};

    IFileOpenDialog* dialog = nullptr;
    const HRESULT create_result = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog));

    if (FAILED(create_result) || dialog == nullptr)
    {
        return result;
    }

    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM |
                                FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR);

    if (!initial_directory.empty())
    {
        IShellItem* folder = nullptr;
        const std::wstring initial_w = initial_directory.wstring();
        if (SUCCEEDED(SHCreateItemFromParsingName(
                initial_w.c_str(), nullptr, IID_PPV_ARGS(&folder))))
        {
            dialog->SetFolder(folder);
            folder->Release();
        }
    }

    if (SUCCEEDED(dialog->Show(nullptr)))
    {
        IShellItem* item = nullptr;
        if (SUCCEEDED(dialog->GetResult(&item)))
        {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
            {
                result = std::filesystem::path(path);
                CoTaskMemFree(path);
            }
            item->Release();
        }
    }

    dialog->Release();
    return result;
}

#else

std::filesystem::path PlatformFileDialog::PickFolder(
    const std::filesystem::path& /*initial_directory*/)
{
    return {};
}

#endif

} // namespace ellindyer::ui::dialogs
