#pragma once

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
  Window();
  ~Window();

  [[gnu::leaf]] inline GLFWwindow *GetWindow() noexcept { return window; }
  [[gnu::hot, gnu::leaf]] inline bool ShouldClose() noexcept {
    return glfwWindowShouldClose(window);
  }

  void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  void Init();

  GLFWwindow *window;
};
} // namespace Enforcer
