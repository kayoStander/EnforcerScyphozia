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

  [[gnu::pure]] GLFWwindow *GetGLFWWindow() const noexcept {
    return window;
  }
  [[gnu::leaf]] VkExtent2D getExtent() const noexcept {
    return {static_cast<u32>(width), static_cast<u32>(height)};
  }
  [[gnu::hot, gnu::pure]] bool ShouldClose() const noexcept {
    return glfwWindowShouldClose(window);
  }
  [[gnu::pure]] bool WasWindowResized() const noexcept {
    return frameBufferResized;
  }

  void ResetWindowResizedFlag() noexcept { frameBufferResized = false; }

  void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) const ;

private:
  static void FramebufferResizedCallback(GLFWwindow *window_, int width,
                                         int height);
  void Init();

  int width, height;
  bool frameBufferResized{false};

  GLFWwindow *window;
};
} // namespace Enforcer
