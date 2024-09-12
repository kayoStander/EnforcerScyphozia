#include "../common/assert.hpp"
#include "../common/config.hpp"
#include "../common/discord.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

  // ASSERT(false);
  // ASSERT(true);

  assert(glfwInit() && "glfwInit returned False");
  assert(glfwVulkanSupported() && "glfw does not support vulkan");

  GLFWwindow *window{glfwCreateWindow(Config::GetMainWindowGeometryWidth(),
                                      Config::GetMainWindowGeometryHeight(),
                                      Config::GetMainWindowName().c_str(), NULL,
                                      NULL)};

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

    RPC.update(Discord::RPCStatus::Playing);

    glfwSwapBuffers(window);
  }

  RPC.stop();
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
