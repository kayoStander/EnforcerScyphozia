#pragma once

#include "../common/types.hpp"

#include <vulkan/vulkan_core.h>
#if __has_include(<GLFW/glfw3.h>)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
#error "No GLFW3 in the recognized in the system"
#endif

namespace Enforcer {
class Window {
public:
  Window(int width, int height);
  ~Window();

  [[gnu::pure]] inline GLFWwindow *GetGLFWWindow() const noexcept {
    return window;
  }
  [[gnu::leaf]] VkExtent2D getExtent() noexcept {
    return {static_cast<u32>(width), static_cast<u32>(height)};
  }
  [[gnu::hot, gnu::pure]] inline bool ShouldClose() noexcept {
    return glfwWindowShouldClose(window);
  }
  [[gnu::pure]] bool inline WasWindowResized() noexcept {
    return frameBufferResized;
  }

  void inline ResetWindowResizedFlag() noexcept { frameBufferResized = false; }

  void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  static void FramebufferResizedCallback(GLFWwindow *window, int width,
                                         int height);
  void Init();

  int width, height;
  bool frameBufferResized{false};

  GLFWwindow *window;
};
} // namespace Enforcer
