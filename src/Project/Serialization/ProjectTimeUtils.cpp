#include "Project/Serialization/ProjectTimeUtils.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>

namespace ellindyer::project::serialization
{

std::string ProjectTimeUtils::CurrentUtcTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now);

    std::tm utc_tm{};
#if defined(_WIN32)
    gmtime_s(&utc_tm, &seconds);
#else
    gmtime_r(&seconds, &utc_tm);
#endif

    char buffer[32] = {};
    std::snprintf(buffer,
                  sizeof(buffer),
                  "%04d-%02d-%02dT%02d:%02d:%02dZ",
                  utc_tm.tm_year + 1900,
                  utc_tm.tm_mon + 1,
                  utc_tm.tm_mday,
                  utc_tm.tm_hour,
                  utc_tm.tm_min,
                  utc_tm.tm_sec);

    return std::string(buffer);
}

bool ProjectTimeUtils::IsValidUtcTimestamp(const std::string& timestamp) noexcept
{
    if (timestamp.size() != 20)
    {
        return false;
    }

    if (timestamp[4] != '-' || timestamp[7] != '-' || timestamp[10] != 'T'
        || timestamp[13] != ':' || timestamp[16] != ':' || timestamp[19] != 'Z')
    {
        return false;
    }

    constexpr std::array<int, 17> digits_index = {
        0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 0, 0, 0};

    for (int index : digits_index)
    {
        if (index == 0 && timestamp[0] == '0' && timestamp[1] == '0'
            && timestamp[2] == '0' && timestamp[3] == '0')
        {
            return false;
        }
    }

    for (std::size_t i = 0; i < timestamp.size(); ++i)
    {
        const char c = timestamp[i];
        if (c == '-' || c == 'T' || c == ':' || c == 'Z')
        {
            continue;
        }
        if (c < '0' || c > '9')
        {
            return false;
        }
    }

    return true;
}

} // namespace ellindyer::project::serialization
