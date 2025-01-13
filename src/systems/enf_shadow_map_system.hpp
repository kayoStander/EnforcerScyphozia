#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
  class ShadowMapSystem {
  public:
    explicit ShadowMapSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~ShadowMapSystem();

    ShadowMapSystem(const ShadowMapSystem &) = delete;
    ShadowMapSystem &operator=(const ShadowMapSystem &) = delete;

    void Update(const FrameInfo &frameInfo) const;

    const std::vector<u32> specializedValues{MAX_LIGHTS};

  private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    Device &device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
} // namespace Enforcer
