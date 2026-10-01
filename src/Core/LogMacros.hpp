#pragma once

#include <string>
#include <string_view>

#include "Core/Error.hpp"
#include "Core/ErrorLogging.hpp"
#include "Core/Logger.hpp"

#define ELLINDYER_LOG_TRACE(message) \
    ::ellindyer::core::GetLogger().LogTrace(message)

#define ELLINDYER_LOG_DEBUG(message) \
    ::ellindyer::core::GetLogger().LogDebug(message)

#define ELLINDYER_LOG_INFO(message) \
    ::ellindyer::core::GetLogger().LogInfo(message)

#define ELLINDYER_LOG_WARNING(message) \
    ::ellindyer::core::GetLogger().LogWarning(message)

#define ELLINDYER_LOG_ERROR(message) \
    ::ellindyer::core::GetLogger().LogError(message)

#define ELLINDYER_LOG_CRITICAL(message) \
    ::ellindyer::core::GetLogger().LogCritical(message)

#define ELLINDYER_LOG_ERROR_OBJECT(error_object) \
    ::ellindyer::core::ErrorLogging::LogError(error_object)

#define ELLINDYER_LOG_ERROR_OBJECT_WITH_CONTEXT(context_text, error_object) \
    ::ellindyer::core::ErrorLogging::LogError(context_text, error_object)
