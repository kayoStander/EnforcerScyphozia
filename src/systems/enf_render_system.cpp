#include "enf_render_system.hpp"
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
  glm::mat4 modelMatrix{1.f};
  glm::mat4 normalMatrix{1.f};
  int imageBind;
};

RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass,
                           VkDescriptorSetLayout globalSetLayout)
    : device{device} {
  LOG_DEBUG(Vulkan, "RenderSystem created");

  CreatePipelineLayout(globalSetLayout);
  CreatePipeline(renderPass);
}

RenderSystem::~RenderSystem() {
  LOG_DEBUG(Vulkan, "RenderSystem destroyed");
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
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

void RenderSystem::CreatePipeline(VkRenderPass renderPass) {
  PipelineConfigInfo pipelineConfig{};
  Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(
      device, "src/shaders/vertex.vert.glsl.spv",
      "src/shaders/fragment.frag.glsl.spv", pipelineConfig);
}

void RenderSystem::RenderGameObjects(FrameInfo &frameInfo) {

  pipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &frameInfo.globalDescriptorSet, 0, nullptr);

  for (auto &keyValue : frameInfo.gameObjects) {
    auto &obj = keyValue.second;
    if (obj.model == nullptr) {
      continue;
    }
    SimplePushConstantData push{};
    push.modelMatrix = obj.transform.mat4();
    push.normalMatrix = obj.transform.normalMatrix();
    push.imageBind = static_cast<s32>(obj.imageBind);

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->Bind(frameInfo.commandBuffer);
    obj.model->Draw(frameInfo.commandBuffer);
  }
}

} // namespace Enforcer
