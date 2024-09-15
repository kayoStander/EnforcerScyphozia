#pragma once

#include "text_formatter.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <iostream>

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
void FmtLogMessage([[maybe_unused]] Class logClass,
                   [[maybe_unused]] Level logLevel,
                   [[maybe_unused]] const char *__restrict__ fileName,
                   [[maybe_unused]] unsigned int lineNum,
                   [[maybe_unused]] const char *function,
                   const char *__restrict__ format, const Args &...args) {
  if (format == nullptr) {
    throw std::invalid_argument("Format cant be nullptr");
  }
  try {
    std::string formated = fmt::format(fmt::runtime(format), args...);

    const Entry entry{/*.timestamp = 1, wip*/
                      .logClass = logClass, .logLevel = logLevel,
                      .fileName = fileName, .lineNum = lineNum,
                      .function = function, .message = formated};
    PrintMessage(entry);
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
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
