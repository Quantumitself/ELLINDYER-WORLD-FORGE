#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace ellindyer::ui::inspector
{

enum class PropertyFieldKind : std::uint8_t
{
    Text,
    MultilineText,
    Integer,
    Float,
    Boolean,
    ReadOnlyText,
    Heading,
    Separator
};

struct PropertyField
{
    PropertyFieldKind                  kind = PropertyFieldKind::Text;
    std::string                        key;
    std::string                        label;
    std::string                        tooltip;
    std::string                        text_value;
    std::int64_t                       int_value   = 0;
    double                             float_value = 0.0;
    bool                               bool_value  = false;
    bool                               read_only   = false;
    std::function<void(const std::string&)>     on_text_changed;
    std::function<void(std::int64_t)>           on_int_changed;
    std::function<void(double)>                 on_float_changed;
    std::function<void(bool)>                   on_bool_changed;
};

[[nodiscard]] PropertyField MakeHeading(std::string label);

[[nodiscard]] PropertyField MakeSeparator(std::string key = std::string{});

[[nodiscard]] PropertyField MakeReadOnlyText(std::string key,
                                             std::string label,
                                             std::string value,
                                             std::string tooltip = std::string{});

[[nodiscard]] PropertyField MakeText(std::string key,
                                     std::string label,
                                     std::string value,
                                     std::function<void(const std::string&)> on_changed,
                                     std::string tooltip = std::string{});

[[nodiscard]] PropertyField MakeMultilineText(std::string key,
                                              std::string label,
                                              std::string value,
                                              std::function<void(const std::string&)> on_changed,
                                              std::string tooltip = std::string{});

[[nodiscard]] PropertyField MakeInteger(std::string key,
                                        std::string label,
                                        std::int64_t value,
                                        std::function<void(std::int64_t)> on_changed,
                                        std::string tooltip = std::string{});

[[nodiscard]] PropertyField MakeFloat(std::string key,
                                      std::string label,
                                      double value,
                                      std::function<void(double)> on_changed,
                                      std::string tooltip = std::string{});

[[nodiscard]] PropertyField MakeBoolean(std::string key,
                                        std::string label,
                                        bool value,
                                        std::function<void(bool)> on_changed,
                                        std::string tooltip = std::string{});

} // namespace ellindyer::ui::inspector
