#pragma once

#include <filesystem>

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Graphics/DirectX11Context.hpp"
#include "UI/Textures/TextureHandle.hpp"

namespace ellindyer::ui::textures
{

class TextureLoader
{
public:
    TextureLoader() = delete;

    [[nodiscard]] static ellindyer::core::Result<TextureHandle> LoadFromFile(
        ellindyer::ui::graphics::DirectX11Context& graphics,
        const std::filesystem::path& path);

    static void Release(TextureHandle& handle);
};

} // namespace ellindyer::ui::textures
