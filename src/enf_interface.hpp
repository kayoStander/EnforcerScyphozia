#pragma once

#include <GLFW/glfw3.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_vulkan.h"
#include "../external/imgui/imgui.h"
#pragma GCC diagnostic pop

namespace Enforcer {
class Interface {
public:
  Interface(GLFWwindow *window) noexcept {}
  static void CreateInterfaceWindow();

private:
  ImGui_ImplVulkan_InitInfo initInfo{};
};
} // namespace Enforcer
