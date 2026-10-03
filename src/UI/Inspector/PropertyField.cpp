#include "UI/Inspector/PropertyField.hpp"

#include <utility>

namespace ellindyer::ui::inspector
{

PropertyField MakeHeading(std::string label)
{
    PropertyField field{};
    field.kind  = PropertyFieldKind::Heading;
    field.label = std::move(label);
    return field;
}

PropertyField MakeSeparator(std::string key)
{
    PropertyField field{};
    field.kind = PropertyFieldKind::Separator;
    field.key  = std::move(key);
    return field;
}

PropertyField MakeReadOnlyText(std::string key,
                               std::string label,
                               std::string value,
                               std::string tooltip)
{
    PropertyField field{};
    field.kind       = PropertyFieldKind::ReadOnlyText;
    field.key        = std::move(key);
    field.label      = std::move(label);
    field.text_value = std::move(value);
    field.tooltip    = std::move(tooltip);
    field.read_only  = true;
    return field;
}

PropertyField MakeText(std::string key,
                       std::string label,
                       std::string value,
                       std::function<void(const std::string&)> on_changed,
                       std::string tooltip)
{
    PropertyField field{};
    field.kind             = PropertyFieldKind::Text;
    field.key              = std::move(key);
    field.label            = std::move(label);
    field.text_value       = std::move(value);
    field.tooltip          = std::move(tooltip);
    field.on_text_changed  = std::move(on_changed);
    return field;
}

PropertyField MakeMultilineText(std::string key,
                                std::string label,
                                std::string value,
                                std::function<void(const std::string&)> on_changed,
                                std::string tooltip)
{
    PropertyField field{};
    field.kind             = PropertyFieldKind::MultilineText;
    field.key              = std::move(key);
    field.label            = std::move(label);
    field.text_value       = std::move(value);
    field.tooltip          = std::move(tooltip);
    field.on_text_changed  = std::move(on_changed);
    return field;
}

PropertyField MakeInteger(std::string key,
                          std::string label,
                          std::int64_t value,
                          std::function<void(std::int64_t)> on_changed,
                          std::string tooltip)
{
    PropertyField field{};
    field.kind            = PropertyFieldKind::Integer;
    field.key             = std::move(key);
    field.label           = std::move(label);
    field.int_value       = value;
    field.tooltip         = std::move(tooltip);
    field.on_int_changed  = std::move(on_changed);
    return field;
}

PropertyField MakeFloat(std::string key,
                        std::string label,
                        double value,
                        std::function<void(double)> on_changed,
                        std::string tooltip)
{
    PropertyField field{};
    field.kind              = PropertyFieldKind::Float;
    field.key               = std::move(key);
    field.label             = std::move(label);
    field.float_value       = value;
    field.tooltip           = std::move(tooltip);
    field.on_float_changed  = std::move(on_changed);
    return field;
}

PropertyField MakeBoolean(std::string key,
                          std::string label,
                          bool value,
                          std::function<void(bool)> on_changed,
                          std::string tooltip)
{
    PropertyField field{};
    field.kind             = PropertyFieldKind::Boolean;
    field.key              = std::move(key);
    field.label            = std::move(label);
    field.bool_value       = value;
    field.tooltip          = std::move(tooltip);
    field.on_bool_changed  = std::move(on_changed);
    return field;
}

} // namespace ellindyer::ui::inspector
