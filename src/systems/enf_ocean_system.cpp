#include "enf_ocean_system.hpp"
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
#include <map>

namespace Enforcer {

  struct OceanPushConstants {
    float speed;
    float height;
    float wlength;
    float time; // make it into the layout instead of push
    bool insideWater;
  };

  OceanSystem::OceanSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
      device{device} {
    LOG_DEBUG(Vulkan, "OceanSystem created");

    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
  }

  OceanSystem::~OceanSystem() {
    LOG_DEBUG(Vulkan, "OceanSystem destroyed");
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
  }

  void OceanSystem::CreatePipelineLayout(const VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(OceanPushConstants);

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

  void OceanSystem::CreatePipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::OceanPipelineConfigInfo(pipelineConfig);
    Pipeline::EnableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(device, "src/shaders/ocean.vert.glsl.spv", "src/shaders/ocean.frag.glsl.spv",
                                          pipelineConfig, specializedValues);
  }

  void OceanSystem::Render(FrameInfo &frameInfo) const {
    pipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frameInfo.globalDescriptorSet, 0, nullptr);


    OceanPushConstants push{};
    push.speed = 1.5f;
    push.height = 10.f;
    push.wlength = 1.2f;
    push.time = frameInfo.frameTime;
    push.insideWater = frameInfo.camera.getPosition().y < 0.f;

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(OceanPushConstants), &push);
  /*const std::shared_ptr<Model> quad{Model::CreateModelFromFile(device, "model/cube.obj")};
  GameObject water{GameObject::CreateGameObject()};
  water.model = quad;
  water.transform.scale = {1000.f,1.f,1000.f};

  quad->Bind(frameInfo.commandBuffer);
  quad->Draw(frameInfo.commandBuffer);
*/}

} // namespace Enforcer
