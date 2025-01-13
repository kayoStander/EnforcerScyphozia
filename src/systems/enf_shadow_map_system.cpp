#include "enf_shadow_map_system.hpp"
#include "../../common/logging/log.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <cassert>
#include <stdexcept>

namespace Enforcer {

  ShadowMapSystem::ShadowMapSystem(Device &device, const VkRenderPass renderPass,
                                   const VkDescriptorSetLayout globalSetLayout) : device{device} {
    LOG_DEBUG(Vulkan, "ShadowMapSystem created");

    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
  }

  ShadowMapSystem::~ShadowMapSystem() {
    LOG_DEBUG(Vulkan, "ShadowMapSystem destroyed");
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
  }

  void ShadowMapSystem::CreatePipelineLayout(const VkDescriptorSetLayout globalSetLayout) {
    const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<u32>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void ShadowMapSystem::CreatePipeline(const VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::ShadowMapPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "src/shaders/shadow_map.vert.glsl.spv",
                                          "src/shaders/shadow_map.frag.glsl.spv", pipelineConfig, specializedValues);
  }

  void ShadowMapSystem::Update(const FrameInfo &frameInfo) const {

    pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frameInfo.globalDescriptorSet, 0, nullptr);

    vkCmdDraw(frameInfo.commandBuffer, 3, 1, 0, 0);
  }

} // namespace Enforcer
