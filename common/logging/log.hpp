#pragma once

#define LOG_CRITICAL(logClass, ...)                                            \
  Common::Logging::FmtLogMessage(Common::Logging::Class::logClass,             \
                                 Common::Logging::Level::Critical,             \
                                 Common::Logging::TrimSourcePath(__FILE__),    \
                                 __LINE__, __func__, __VA_ARGS__)
