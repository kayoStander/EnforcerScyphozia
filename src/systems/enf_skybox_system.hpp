#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
  class SkyboxSystem {
  public:
    explicit SkyboxSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SkyboxSystem();

    SkyboxSystem(const SkyboxSystem &) = delete;
    SkyboxSystem &operator=(const SkyboxSystem &) = delete;

    void RenderSkybox(const FrameInfo &frameInfo) const;

    const std::vector<u32> specializedValues{MAX_LIGHTS, MAX_TEXTURE};

  private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    Device &device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
} // namespace Enforcer
