#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>
#include <variant>

namespace Enforcer {
class SkyboxSystem {
public:
  explicit SkyboxSystem(Device &device, VkRenderPass renderPass,
                        VkDescriptorSetLayout globalSetLayout);
  ~SkyboxSystem();

  SkyboxSystem(const SkyboxSystem &) = delete;
  SkyboxSystem &operator=(const SkyboxSystem &) = delete;

  void RenderSkybox(FrameInfo &frameInfo);

  const std::vector<std::variant<u32, float>> specializedValues{};

private:
  void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void CreatePipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace Enforcer
