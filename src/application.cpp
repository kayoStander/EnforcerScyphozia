#include "application.hpp"

namespace Enforcer {
void Application::Run() {

  while (!window.ShouldClose()) {
    glfwPollEvents();

    glfwSwapBuffers(window.GetWindow());
  }
}
} // namespace Enforcer
