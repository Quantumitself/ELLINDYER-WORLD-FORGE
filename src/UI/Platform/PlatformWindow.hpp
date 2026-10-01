#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

#include <windows.h>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::ui::platform
{

struct PlatformWindowDescription
{
    std::wstring title = L"Ellindyer World Forge";
    std::uint32_t width = 1280U;
    std::uint32_t height = 800U;
    std::uint32_t min_width = 640U;
    std::uint32_t min_height = 400U;
    bool resizable = true;
    bool centered  = true;
};

class PlatformWindow
{
public:
    using ResizeCallback = std::function<void(std::uint32_t, std::uint32_t)>;
    using CloseCallback  = std::function<void()>;

    PlatformWindow();
    ~PlatformWindow();

    PlatformWindow(const PlatformWindow&) = delete;
    PlatformWindow& operator=(const PlatformWindow&) = delete;
    PlatformWindow(PlatformWindow&&) noexcept = delete;
    PlatformWindow& operator=(PlatformWindow&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Create(
        const PlatformWindowDescription& description);

    void Destroy();

    [[nodiscard]] bool IsValid() const noexcept;

    [[nodiscard]] HWND GetHandle() const noexcept;

    [[nodiscard]] std::uint32_t GetWidth() const noexcept;

    [[nodiscard]] std::uint32_t GetHeight() const noexcept;

    void Show();

    void Hide();

    void SetTitle(std::wstring_view title);

    void SetResizeCallback(ResizeCallback callback);

    void SetCloseCallback(CloseCallback callback);

    [[nodiscard]] bool PumpMessages();

    void RequestClose();

    [[nodiscard]] bool ShouldClose() const noexcept;

private:
    static LRESULT CALLBACK WindowProcedureThunk(HWND window,
                                                 UINT message,
                                                 WPARAM wparam,
                                                 LPARAM lparam);

    LRESULT WindowProcedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    [[nodiscard]] static std::wstring RegisterWindowClass();

    HWND          window_handle_  = nullptr;
    std::wstring  class_name_;
    std::uint32_t width_          = 0;
    std::uint32_t height_         = 0;
    std::uint32_t min_width_      = 0;
    std::uint32_t min_height_     = 0;
    bool          resizable_      = true;
    bool          should_close_   = false;

    ResizeCallback resize_callback_;
    CloseCallback  close_callback_;
};

} // namespace ellindyer::ui::platform
