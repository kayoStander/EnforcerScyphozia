#pragma once

#include "../enf_device.hpp"
#include "../enf_frame_info.hpp"
#include "../enf_pipeline.hpp"

#include <memory>

namespace Enforcer {
  class OceanSystem {
  public:
    OceanSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~OceanSystem();

    OceanSystem(const OceanSystem &) = delete;
    OceanSystem &operator=(const OceanSystem &) = delete;

    void Render(FrameInfo &frameInfo) const;

    const std::vector<u32> specializedValues{MAX_LIGHTS};

  private:
    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

    Device &device;

    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
  };
} // namespace Enforcer
