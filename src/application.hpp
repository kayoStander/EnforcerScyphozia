#pragma once

#include "enf_descriptors.hpp"
#include "enf_device.hpp"
#include "enf_game_object.hpp"
#include "enf_renderer.hpp"
#include "enf_window.hpp"

#include <memory>
#include <unordered_map>

#define WIDTH 800
#define HEIGHT 600

namespace Enforcer {
class Application {
public:
  explicit Application();
  ~Application();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void Run();

private:
  void LoadGameObjects();

  Window window{WIDTH, HEIGHT};
  Device device{window};
  Renderer renderer{window, device};

  // order of declaration matters
  std::unique_ptr<DescriptorPool> globalPool{};
  std::unordered_map<u32, GameObject> gameObjects;
};
} // namespace Enforcer
