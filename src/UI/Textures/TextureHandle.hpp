#pragma once

#include <cstdint>

struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace ellindyer::ui::textures
{

struct TextureHandle
{
    ID3D11ShaderResourceView* view  = nullptr;
    ID3D11Texture2D*          texture = nullptr;
    std::uint32_t             width  = 0;
    std::uint32_t             height = 0;

    [[nodiscard]] bool IsValid() const noexcept
    {
        return view != nullptr && texture != nullptr && width > 0 && height > 0;
    }
};

} // namespace ellindyer::ui::textures
