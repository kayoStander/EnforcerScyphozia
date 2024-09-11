#include "../common/discord.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

Discord::RPC RPC{};

[[gnu::leaf]] void GLFWKeyCallback(GLFWwindow *window, const int key,
                                   [[maybe_unused]] const int scancode,
                                   const int action,
                                   [[maybe_unused]] const int mods) {
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

  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, GLFWKeyCallback);

  RPC.init();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glfwSwapBuffers(window);
  }

  RPC.stop();
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
