#include "enf_render_system.hpp"
#include "../common/logging/log.hpp"

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
  glm::mat4 transform{1.f};
  alignas(16) glm::vec3 color{};
};

RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass)
    : device{device} {
  LOG_DEBUG(Vulkan, "RenderSystem created");

  CreatePipelineLayout();
  CreatePipeline(renderPass);
}

RenderSystem::~RenderSystem() {
  LOG_DEBUG(Vulkan, "RenderSystem destroyed");
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::CreatePipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
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

void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer,
                                     std::vector<GameObject> &gameObjectss,
                                     const Camera &camera) {

  pipeline->bind(commandBuffer);

  glm::mat4 projectionView = camera.GetProjection() * camera.GetView();

  for (auto &obj : gameObjectss) {
    SimplePushConstantData push{};
    push.color = obj.color;
    push.transform = projectionView * obj.transform.mat4();

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->Bind(commandBuffer);
    obj.model->Draw(commandBuffer);
  }
}

} // namespace Enforcer
