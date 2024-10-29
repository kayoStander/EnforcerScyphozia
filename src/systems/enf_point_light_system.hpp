#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
class PointLightSystem {
public:
  explicit PointLightSystem(Device &device, VkRenderPass renderPass,
                            VkDescriptorSetLayout globalSetLayout);
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem &) = delete;
  PointLightSystem &operator=(const PointLightSystem &) = delete;

  void Update(FrameInfo &frameInfo,
              DefaultUniformBufferObject &uniformBufferObject);
  void Render(FrameInfo &frameInfo);

  const std::vector<u32> specializedValues{};

private:
  void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void CreatePipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace Enforcer
