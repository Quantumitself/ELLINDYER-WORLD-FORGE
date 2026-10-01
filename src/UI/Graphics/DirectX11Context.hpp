#pragma once

#include <cstdint>
#include <memory>

#include <windows.h>
#include <d3d11.h>

#include "Core/Error.hpp"
#include "Core/Result.hpp"

namespace ellindyer::ui::graphics
{

struct DirectX11ContextDescription
{
    HWND window = nullptr;
    std::uint32_t width  = 0;
    std::uint32_t height = 0;
    bool vsync = true;
    bool enable_debug_layer = false;
};

class DirectX11Context
{
public:
    DirectX11Context();
    ~DirectX11Context();

    DirectX11Context(const DirectX11Context&) = delete;
    DirectX11Context& operator=(const DirectX11Context&) = delete;
    DirectX11Context(DirectX11Context&&) noexcept = delete;
    DirectX11Context& operator=(DirectX11Context&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Create(
        const DirectX11ContextDescription& description);

    void Destroy();

    [[nodiscard]] bool IsValid() const noexcept;

    [[nodiscard]] ID3D11Device*        GetDevice() const noexcept;
    [[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const noexcept;
    [[nodiscard]] IDXGISwapChain*      GetSwapChain() const noexcept;
    [[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const noexcept;

    [[nodiscard]] ellindyer::core::Result<void> Resize(std::uint32_t width,
                                                       std::uint32_t height);

    void BeginFrame(float clear_r, float clear_g, float clear_b, float clear_a);

    [[nodiscard]] ellindyer::core::Result<void> Present();

    [[nodiscard]] std::uint32_t GetWidth() const noexcept;

    [[nodiscard]] std::uint32_t GetHeight() const noexcept;

private:
    [[nodiscard]] ellindyer::core::Result<void> CreateDeviceAndSwapChain(
        const DirectX11ContextDescription& description);

    [[nodiscard]] ellindyer::core::Result<void> CreateRenderTarget();

    void ReleaseRenderTarget();

    void ReleaseAll();

    ID3D11Device*            device_             = nullptr;
    ID3D11DeviceContext*     device_context_     = nullptr;
    IDXGISwapChain*          swap_chain_         = nullptr;
    ID3D11RenderTargetView*  render_target_view_ = nullptr;

    std::uint32_t            width_  = 0;
    std::uint32_t            height_ = 0;
    bool                     vsync_  = true;
};

} // namespace ellindyer::ui::graphics
