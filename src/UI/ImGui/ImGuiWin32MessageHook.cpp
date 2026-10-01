#include "UI/ImGui/ImGuiWin32MessageHook.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace ellindyer::ui::imgui_layer
{

bool ImGuiWin32MessageHook::HandleMessage(HWND window,
                                          UINT message,
                                          WPARAM wparam,
                                          LPARAM lparam)
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        return false;
    }
    const LRESULT result = ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam);
    return result != 0;
}

} // namespace ellindyer::ui::imgui_layer
