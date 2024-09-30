#pragma once

#include "enf_camera.hpp"
#include "enf_device.hpp"
#include "enf_game_object.hpp"
#include "enf_pipeline.hpp"
#include "enf_renderer.hpp"
#include "enf_window.hpp"

#include <memory>
#include <vector>

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
  void CreatePipelineLayout();
  void CreatePipeline();
  void RenderGameObjects(VkCommandBuffer commandBuffer, const Camera &camera);

  Window window{WIDTH, HEIGHT};
  Device device{window};
  Renderer renderer{window, device};

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<GameObject> gameObjects;
};
} // namespace Enforcer
