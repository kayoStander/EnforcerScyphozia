#include "enf_window.hpp"
#include "../common/assert.hpp"
#include "../common/config.hpp"

#include <GLFW/glfw3.h>

[[gnu::leaf]] void ErrorCallback(int error, const char *description) {
  LOG_ERROR(GLFW, "{} {}", error, description);
}

namespace Enforcer {
Window::Window() {
  glfwSetErrorCallback(ErrorCallback);

  Init();
}
Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::Init() {
  ASSERT_LOG(glfwInit(), "glfwInit returned False");
  ASSERT_LOG(glfwVulkanSupported(), "GLFW does not support vulkan");

  // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(Config::GetMainWindowGeometryWidth(),
                       Config::GetMainWindowGeometryHeight(),
                       Config::GetMainWindowName().c_str(), nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Window wasn't generated");
  }
  glfwMakeContextCurrent(window);

  LOG_INFO(GLFW, "Window created");
}
} // namespace Enforcer
