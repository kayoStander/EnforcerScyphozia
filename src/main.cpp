#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#ifdef DEBUG
#define CHECKFORERR(x) if (!(x))
#else
#define CHECKFORERR(x)
#endif

[[gnu::leaf]] void GLFWKeyCallback(GLFWwindow *window, const int key,
                                   const int scancode, const int action,
                                   const int mods) {
  if ((key == GLFW_KEY_SPACE && (action == GLFW_PRESS))) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main([[maybe_unused]] const int argc, [[maybe_unused]] const char *argv[]) {
  if (glfwInit()) {
    return 1;
  }

  assert(!glfwInit() && "glfwInit returned False");
  assert(!glfwVulkanSupported() && "glfw doest not support vulkan");

  GLFWwindow *window{glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                      "Vk-Enforcer-0.0", NULL, NULL)};

  while (!glfwWindowShouldClose(window)) {
  }
}
