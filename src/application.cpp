#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_game_object.hpp"
#include "enf_model.hpp"
#include "enf_pipeline.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <memory>

Discord::RPC RPC{};

namespace Enforcer {

struct SimplePushConstantData {
  glm::mat2 transform{1.f}; // 1 0 0 1 // mat2{1.f} * v = v
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

Application::Application() {
  LoadGameObjects();
  CreatePipelineLayout();
  CreatePipeline();
}

Application::~Application() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void Application::Run() {
  RPC.Init();

  LOG_DEBUG(GLFW, "Loop started");
  LOG_DEBUG(Vulkan, "maxPushCostantsSize => {}",
            device.properties.limits.maxPushConstantsSize);

  while (!window.ShouldClose()) {
    glfwPollEvents();
    if (VkCommandBuffer commandBuffer = renderer.BeginFrame()) {
      renderer.BeginSwapChainRenderPass(commandBuffer);
      RenderGameObjects(commandBuffer);
      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }
    RPC.Update(Discord::RPCStatus::Playing);
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}
void Application::LoadGameObjects() {
  const std::vector<Model::Vertex> vertices{{{0.f, -.5f}, {1.f, 0.f, 0.f}},
                                            {{.5f, .5f}, {0.f, 1.f, 0.f}},
                                            {{-.5f, .5f}, {0.f, 0.f, 1.f}}};

#ifdef DEBUG
  for (const Model::Vertex i : vertices) {
    LOG_TRACE(Vulkan, "Vertices => {}x:{}y", i.position.x, i.position.y);
  }
#endif

  std::shared_ptr<Model> model = std::make_shared<Model>(device, vertices);

  GameObject triangle = GameObject::CreateGameObject();
  triangle.model = model;
  triangle.color = {.1f, .8f, .1f};
  triangle.transform2D.translation.x = .2f;
  triangle.transform2D.scale = {2.f, .5f};
  triangle.transform2D.rotation = .25f * -glm::two_pi<float>(); // rad

  gameObjects.push_back(std::move(triangle));
}

void Application::CreatePipelineLayout() {
  // swapchain"); ASSERT_LOG(pipelineLayout != nullptr, "Cannot create pipeline
  // before layout");

  VkPushConstantRange pushCostantRange{};
  pushCostantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushCostantRange.offset = 0;
  pushCostantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushCostantRange;
  if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipelineLayout.");
  }
}

void Application::CreatePipeline() {
  PipelineConfigInfo pipelineConfig{};
  Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderer.GetSwapChainRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(
      device, "src/shaders/vertex.vert.glsl.spv",
      "src/shaders/fragment.frag.glsl.spv", pipelineConfig);
}

void Application::RenderGameObjects(VkCommandBuffer commandBuffer) {
  // atualized every frame

  pipeline->bind(commandBuffer);

  for (auto &obj : gameObjects) {
    obj.transform2D.rotation =
        glm::mod(obj.transform2D.rotation + .01f, glm::two_pi<float>());

    SimplePushConstantData push{};
    push.offset = obj.transform2D.translation;
    push.color = obj.color;
    push.transform = obj.transform2D.mat2();

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);

    obj.model->Bind(commandBuffer);
    obj.model->Draw(commandBuffer);
  }
}
} // namespace Enforcer
