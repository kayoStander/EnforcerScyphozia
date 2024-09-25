#include "enf_window.hpp"
#include "../common/assert.hpp"
#include "../common/config.hpp"

[[gnu::leaf]] void ErrorCallback(int error, const char *description) {
  LOG_ERROR(GLFW, "{} {}", error, description);
}

namespace Enforcer {
Window::Window() {
  glfwSetErrorCallback(ErrorCallback);

  Init();
}
Window::~Window() {
  LOG_DEBUG(GLFW, "Window closed");

  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface");
    ASSERT_LOG(false, "Failed to create window surface");
  }
}

void Window::Init() {
  ASSERT_LOG(glfwInit(), "glfwInit returned False");
  ASSERT_LOG(glfwVulkanSupported(), "GLFW does not support vulkan");

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(static_cast<int>(Config::GetMainWindowGeometryWidth()),
                       static_cast<int>(Config::GetMainWindowGeometryHeight()),
                       Config::GetMainWindowName().c_str(), nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Window wasn't generated");
  }

  LOG_INFO(GLFW, "Window created");
}
} // namespace Enforcer
