#include "application.hpp"
#include "../common/discord.hpp"

Discord::RPC RPC{};

namespace Enforcer {
void Application::Run() {
  RPC.Init();
  while (!window.ShouldClose()) {
    glfwPollEvents();
    RPC.Update(Discord::RPCStatus::Playing);
  }
  RPC.Stop();
}
} // namespace Enforcer
