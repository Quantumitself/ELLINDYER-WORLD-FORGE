#pragma once

#include <cstdint>

namespace ellindyer::ui::dialogs
{

enum class DialogOutcome : std::uint8_t
{
    Open,
    Accepted,
    Cancelled
};

struct DialogResult
{
    DialogOutcome outcome = DialogOutcome::Open;

    [[nodiscard]] bool IsOpen() const noexcept
    {
        return outcome == DialogOutcome::Open;
    }

    [[nodiscard]] bool IsAccepted() const noexcept
    {
        return outcome == DialogOutcome::Accepted;
    }

    [[nodiscard]] bool IsCancelled() const noexcept
    {
        return outcome == DialogOutcome::Cancelled;
    }
};

} // namespace ellindyer::ui::dialogs
