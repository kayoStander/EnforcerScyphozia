// #include "../common/assert.hpp"
#include "../common/config.hpp"
#include "../common/discord.hpp"
#include "../common/logging/log.hpp"

#include <cassert>

#if __has_include(<GLFW/glfw3.h>)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
#error "How the fuck you ran the code without glfw?"
#endif

static_assert(__cplusplus >= 202100L, "The required version is C++20 or more.");

Discord::RPC RPC{};

[[gnu::leaf]] void GLFWKeyCallback(GLFWwindow *window, const int key,
                                   [[maybe_unused]] const int scancode,
                                   const int action,
                                   [[maybe_unused]] const int mods) {
  if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS)) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main([[maybe_unused]] const int argc, [[maybe_unused]] const char *argv[]) {
  assert(glfwInit() && "glfwInit returned False");
  assert(glfwVulkanSupported() && "glfw does not support vulkan");

  LOG_DEBUG(Common, "{} {}", "Hello", "World");
  LOG_INFO(Logging, "{}", "This is a logging message");
  LOG_CRITICAL(Core, "This is a critical message");

  GLFWwindow *window{glfwCreateWindow(Config::GetMainWindowGeometryWidth(),
                                      Config::GetMainWindowGeometryHeight(),
                                      Config::GetMainWindowName().c_str(), NULL,
                                      NULL)};

  if (!window) {
    fprintf(stderr, "Failed to create GLFW Window\n");
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
