#pragma once

#if __has_include(<GLFW/glfw3.h>)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
#error "No GLFW3 in the recognized in the system"
#endif

#include <GLFW/glfw3.h>
namespace Enforcer {
class Window {
public:
  Window();
  ~Window();

  inline GLFWwindow *GetWindow() noexcept { return window; }
  inline bool ShouldClose() noexcept { return glfwWindowShouldClose(window); }

private:
  void Init();

  GLFWwindow *window;
};
} // namespace Enforcer
