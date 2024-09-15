#pragma once

#include "entry.hpp"
#include "text_formatter.hpp"
#include "types.hpp"

#include <algorithm>
#include <fmt/core.h>

namespace Common::Logging {
constexpr const char *TrimSourcePath(std::string_view source) {
  const auto rfind = [source](const std::string_view match) {
    return source.rfind(match) == source.npos
               ? 0
               : (source.rfind(match) + match.size());
  };
  u32 idx = std::max({rfind("/"), rfind("\\")});
  return source.data() + idx;
}
template <typename... Args>
void FmtLogMessage(Class logClass, Level logLevel,
                   const char *__restrict__ fileName, unsigned int lineNum,
                   const char *function, const char *__restrict__ format,
                   const Args &...args) {
  std::string formated = fmt::format(fmt::runtime(format), args...);
  const Entry entry{/*.timestamp = 1, wip*/
                    .logClass = logClass, .logLevel = logLevel,
                    .fileName = fileName, .lineNum = lineNum,
                    .function = function, .message = std::move(formated)};
  PrintMessage(entry);
}
} // namespace Common::Logging

#define LOG_DEBUG(logClass, ...)                                               \
  Common::Logging::FmtLogMessage(Common::Logging::Class::logClass,             \
                                 Common::Logging::Level::Debug,                \
                                 Common::Logging::TrimSourcePath(__FILE__),    \
                                 __LINE__, __func__, __VA_ARGS__)

#define LOG_INFO(logClass, ...)                                                \
  Common::Logging::FmtLogMessage(Common::Logging::Class::logClass,             \
                                 Common::Logging::Level::Info,                 \
                                 Common::Logging::TrimSourcePath(__FILE__),    \
                                 __LINE__, __func__, __VA_ARGS__)

#define LOG_CRITICAL(logClass, ...)                                            \
  Common::Logging::FmtLogMessage(Common::Logging::Class::logClass,             \
                                 Common::Logging::Level::Critical,             \
                                 Common::Logging::TrimSourcePath(__FILE__),    \
                                 __LINE__, __func__, __VA_ARGS__)
