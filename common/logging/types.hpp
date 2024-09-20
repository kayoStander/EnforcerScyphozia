#pragma once

#include "../types.hpp"

// Specifies the level of the logging
namespace Common::Logging {
enum class Level : u8 {
  Trace,
  Debug,
  Info,
  Warning,
  Error,
  Critical, // D:
  Count     // total number of levels
};

// Specifies where the logging was used for
enum class Class : u8 {
  Logging, // For logging folder
  Common,  // For common folder
  Core,
  Debug,
  Discord,
  Config, // For config file
  GLFW,   // GLFW related events
  Vulkan, // Vulkan related events
  Count   // total number of classes
};
} // namespace Common::Logging
