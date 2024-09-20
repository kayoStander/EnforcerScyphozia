#pragma once

#include "enf_window.hpp"

namespace Enforcer {
class Application {
public:
  void Run();

private:
  Window window{};
};
} // namespace Enforcer
