#include "text_formatter.hpp"
#include "filter.hpp"
#include <fmt/core.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Common::Logging {
std::string FormatLogMessage(const Entry &entry) {
  /*[[maybe_unused]] const u32 timeSeconds =
      static_cast<u32>(entry.timestamp.count() / 1000000);
  [[maybe_unused]] const u32 timeFractional =
      static_cast<u32>(entry.timestamp.count() % 1000000);
  */

  const char *className{GetLogClassName(entry.logClass)};
  const char *levelName{GetLevelName(entry.logLevel)};

  return fmt::format("[{}:{}] {}:{}:{} -> {}", levelName, className,
                     entry.fileName, entry.function, entry.lineNum,
                     entry.message);
}

inline void PrintMessage(const Entry &entry) noexcept {
  const auto str{FormatLogMessage(entry).append(1, '\n')};
  if (fputs(str.c_str(), stdout) == EOF) {
    fprintf(stderr, "Failed to write into stdout");
  }
}

// fuck you windows
void PrintColoredMessage(const Entry &entry) {
#ifdef _WIN32
  HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
  if (consoleHandle == INVALID_HANDLE_VALUE) {
    return;
  }

  CONSOLE_SCREEN_BUFFER_INFO originalInfo{};
  GetConsoleScreenBufferInfo(consoleHandle, &originalInfo);

  WORD color{0};
  switch (entry.logLevel) {
  case Level::Trace:
    color = FOREGROUND_INTENSITY;
    break;
  case Level::Debug:
    color = FOREGROUND_GREEN | FOREGROUND_BLUE;
    break;
  case Level::Info:
    color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    break;
  case Level::Warning:
    color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    break;
  case Level::Error:
    color = FOREGROUND_RED | FOREGROUND_INTENSITY;
    break;
  case Level::Critical:
    color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    break;
  case Level::Count:
    [[fallthrough]];
  default:
    break;
  }
#else
#define ESC "\x1b"
  const char *color{""};
  switch (entry.logLevel) {
  case Level::Trace:
    color = ESC "[1;30m";
    break;
  case Level::Debug:
    color = ESC "[0;36m";
    break;
  case Level::Info:
    color = ESC "[0;37m";
    break;
  case Level::Warning:
    color = ESC "[1;33m";
    break;
  case Level::Error:
    color = ESC "[1;31m";
    break;
  case Level::Critical:
    color = ESC "[1;35m";
    break;
  case Level::Count:
    [[fallthrough]];
  default:
    break;
  }
  fputs(color, stdout);
#endif

  PrintMessage(entry);

#ifdef _WIN32
  SetConsoleTextAttribute(console_handle, original_info.wAttributes);
#else
  fputs(ESC "[0m", stdout);
#undef ESC
#endif
}
} // namespace Common::Logging
