#include "types.hpp"
#include <chrono>
namespace Common::Logging {
struct Entry {
  std::chrono::microseconds timestamp;
  Class logClass{};
  Level logLevel{};
  const char *fileName{nullptr};
  u32 lineNum{0};
  std::string function;
  std::string message;
};
} // namespace Common::Logging
