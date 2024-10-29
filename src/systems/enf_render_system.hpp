#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
class RenderSystem {
public:
  explicit RenderSystem(Device &device, VkRenderPass renderPass,
                        VkDescriptorSetLayout globalSetLayout);
  ~RenderSystem();

  RenderSystem(const RenderSystem &) = delete;
  RenderSystem &operator=(const RenderSystem &) = delete;

  void RenderGameObjects(FrameInfo &frameInfo);

  const std::vector<u32> specializedValues{};

private:
  void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void CreatePipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace Enforcer
