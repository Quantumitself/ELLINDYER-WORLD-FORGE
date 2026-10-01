#pragma once

#include "Core/Error.hpp"
#include "Core/Result.hpp"
#include "UI/Branding/BrandingAssets.hpp"
#include "UI/Graphics/DirectX11Context.hpp"
#include "UI/Textures/TextureHandle.hpp"

namespace ellindyer::ui::branding
{

class LogoTexture
{
public:
    LogoTexture();
    ~LogoTexture();

    LogoTexture(const LogoTexture&) = delete;
    LogoTexture& operator=(const LogoTexture&) = delete;
    LogoTexture(LogoTexture&&) noexcept = delete;
    LogoTexture& operator=(LogoTexture&&) noexcept = delete;

    [[nodiscard]] ellindyer::core::Result<void> Load(
        ellindyer::ui::graphics::DirectX11Context& graphics,
        const BrandingAssets& assets);

    void Release();

    [[nodiscard]] bool IsLoaded() const noexcept;

    [[nodiscard]] const ellindyer::ui::textures::TextureHandle& GetTexture() const noexcept;

private:
    ellindyer::ui::textures::TextureHandle texture_{};
    bool loaded_ = false;
};

} // namespace ellindyer::ui::branding
