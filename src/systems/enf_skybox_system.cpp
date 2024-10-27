#include "enf_skybox_system.hpp"
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

struct SimplePushConstantData {
  glm::vec3 fogColor;
  float fogStart;
  float fogEnd;
};

SkyboxSystem::SkyboxSystem(Device &device, VkRenderPass renderPass,
                           VkDescriptorSetLayout globalSetLayout)
    : device{device} {
  LOG_DEBUG(Vulkan, "SkyboxSystem created");

  CreatePipelineLayout(globalSetLayout);
  CreatePipeline(renderPass);
}

SkyboxSystem::~SkyboxSystem() {
  LOG_DEBUG(Vulkan, "SkyboxSystem destroyed");
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void SkyboxSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<u32>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void SkyboxSystem::CreatePipeline(VkRenderPass renderPass) {
  PipelineConfigInfo pipelineConfig{};
  Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.attributeDescriptions.clear();
  pipelineConfig.bindingDescriptions.clear();
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(
      device, "src/shaders/skybox.vert.glsl.spv",
      "src/shaders/skybox.frag.glsl.spv", pipelineConfig);
}

void SkyboxSystem::RenderSkybox(FrameInfo &frameInfo) {

  pipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);

  SimplePushConstantData push{};
  push.fogColor = glm::vec3(.1, .1, .1);
  push.fogStart = 50.f;
  push.fogEnd = 150.f;

  vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                     VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                     sizeof(SimplePushConstantData), &push);
  vkCmdDraw(frameInfo.commandBuffer, 36, 1, 0, 0);
}

} // namespace Enforcer
