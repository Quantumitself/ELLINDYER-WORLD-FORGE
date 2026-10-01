#include <spdlog/spdlog.h>
#include <spdlog/version.h>
#if defined(SPDLOG_COMPILED_LIB)
#pragma message("SPDLOG_COMPILED_LIB is defined")
#endif
#if defined(SPDLOG_HEADER_ONLY)
#pragma message("SPDLOG_HEADER_ONLY is defined")
#endif
int main() { return 0; }
