#include "text_formatter.hpp"
#include "filter.hpp"
#include <fmt/core.h>

namespace Common::Logging {
std::string FormatLogMessage(const Entry &entry) {
  const u32 timeSeconds = static_cast<u32>(entry.timestamp.count() / 1000000);
  const u32 timeFractional =
      static_cast<u32>(entry.timestamp.count() % 1000000);

  [[maybe_unused]] const char *className{GetLogClassName(entry.logClass)};
  [[maybe_unused]] const char *levelName{GetLevelName(entry.logLevel)};

  return fmt::format("[{}] - {} - {}:{} -> {}", className, levelName,
                     entry.function, entry.lineNum, entry.message);
}

void PrintMessage(const Entry &entry) {
  const auto str{FormatLogMessage(entry).append(1, '\n')};
  fputs(str.c_str(), stdout);
}
} // namespace Common::Logging
