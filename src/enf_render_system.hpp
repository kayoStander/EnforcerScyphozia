#pragma once

#include "enf_camera.hpp"
#include "enf_device.hpp"
#include "enf_game_object.hpp"
#include "enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
class RenderSystem {
public:
  explicit RenderSystem(Device &device, VkRenderPass renderPass);
  ~RenderSystem();

  RenderSystem(const RenderSystem &) = delete;
  RenderSystem &operator=(const RenderSystem &) = delete;

  void RenderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<GameObject> &gameObjects,
                         const Camera &camera);

private:
  void CreatePipelineLayout();
  void CreatePipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace Enforcer
