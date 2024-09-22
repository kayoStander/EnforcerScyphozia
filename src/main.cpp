#include "../common/logging/log.hpp"
#include "application.hpp"

#include <cassert>
#include <exception>

static_assert(__cplusplus >= 202100L, "The required version is C++20 or more.");

int main([[maybe_unused]] const int argc, [[maybe_unused]] const char *argv[]) {

  Enforcer::Application app{};
  try {
    app.Run();
  } catch (const std::exception &e) {
    LOG_CRITICAL(Core, "Exception ocurred: {}", e.what());
    return EXIT_FAILURE;
  }

  LOG_INFO(Debug, "Ended session!");

  return EXIT_SUCCESS;
}
