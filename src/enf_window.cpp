#include "enf_window.hpp"
#include "../common/assert.hpp"
#include "../common/config.hpp"

[[gnu::leaf]] void ErrorCallback(int error, const char *description) { LOG_ERROR(GLFW, "{} {}", error, description); }

namespace Enforcer {
  Window::Window(const int width, const int height) : width{width}, height{height}, window{} { Init(); }
  Window::~Window() {
    LOG_DEBUG(GLFW, "Window closed");

    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::CreateWindowSurface(const VkInstance instance, VkSurfaceKHR *surface) const {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface");
      ASSERT_LOG(false, "Failed to create window surface");
    }
  }

  void Window::Init() {
    ASSERT_LOG(glfwInit(), "glfwInit returned False");
    ASSERT_LOG(glfwVulkanSupported(), "GLFW does not support vulkan");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, Config::GetMainWindowName().c_str(), nullptr, nullptr);

    if (!window) {
      glfwTerminate();
      throw std::runtime_error("Window wasn't generated");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizedCallback);
    // glfwSetErrorCallback(ErrorCallback);

    LOG_INFO(GLFW, "Window created");
  }

  void Window::FramebufferResizedCallback(GLFWwindow *window_, const int width, const int height) {
    const auto window = static_cast<Window *>(glfwGetWindowUserPointer(window_));
    ASSERT_LOG(window, "Window wasn't generated");

    if (window) {
      window->frameBufferResized = true;
      window->width = width;
      window->height = height;
      LOG_WARNING(GLFW, "{}x : {}y", width, height);
      return;
    }
  }
} // namespace Enforcer
