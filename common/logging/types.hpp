#pragma once

#include "../types.hpp"

namespace Common::Logging {
enum class Level : u8 { Trace, Debug, Info, Warning, Error, Critical, Count };

enum class Class : u8 {
  Logging, // For logging folder
  Common,  // For common folder
  Core,
  Debug,
  Count
};
} // namespace Common::Logging
