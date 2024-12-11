#include "enf_point_light_system.hpp"
#include <map>
#include "../../common/assert.hpp"
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

namespace Enforcer {

  struct PointLightPushConstants {
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
  };

  PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
      device{device} {
    LOG_DEBUG(Vulkan, "PointLightSystem created");

    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
  }

  PointLightSystem::~PointLightSystem() {
    LOG_DEBUG(Vulkan, "PointLightSystem destroyed");
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
  }

  void PointLightSystem::CreatePipelineLayout(const VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

    const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<u32>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    VK_CHECK_RESULT_LOG(vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
                        "Failed to create pipeline layout!");
  }

  void PointLightSystem::CreatePipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
    Pipeline::EnableAlphaBlending(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "src/shaders/point_light.vert.glsl.spv",
                                          "src/shaders/point_light.frag.glsl.spv", pipelineConfig, specializedValues);
  }

  void PointLightSystem::Update(const FrameInfo &frameInfo, DefaultUniformBufferObject &uniformBufferObject) {
    glm::mat4 rotateLight{glm::rotate(glm::mat4{1.f}, frameInfo.frameTime, {0.f, -1.f, 0.f})};

    int lightIndex{0};
    for (auto &keyValue: frameInfo.gameObjects) {
      GameObject &obj{keyValue.second};
      if (obj.pointLight == nullptr) {
        continue;
      }

      ASSERT_LOG(lightIndex < MAX_LIGHTS, "number of lights is bigger than the limit");

      // obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

      uniformBufferObject.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
      uniformBufferObject.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
      lightIndex++;
    }
    uniformBufferObject.numLights = lightIndex;
  }

  void PointLightSystem::Render(FrameInfo &frameInfo) {
    std::map<float, u32> sorted;
    for (auto &keyValue: frameInfo.gameObjects) {
      GameObject &obj{keyValue.second};
      if (obj.pointLight == nullptr) {
        continue;
      }
      glm::vec3 offset{frameInfo.camera.getPosition() - obj.transform.translation};
      float distanceSquared{glm::dot(offset, offset)};
      sorted[distanceSquared] = obj.GetId();
    }

    pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frameInfo.globalDescriptorSet, 0, nullptr);

    for (std::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
      GameObject &obj{frameInfo.gameObjects.at(it->second)};

      PointLightPushConstants push{};
      push.position = glm::vec4(obj.transform.translation, 1.f);
      push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
      push.radius = obj.transform.scale.x;

      vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants),
                         &push);
      vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
  }

} // namespace Enforcer
