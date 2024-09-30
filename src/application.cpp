#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_game_object.hpp"
#include "enf_model.hpp"
#include "enf_pipeline.hpp"
#include "keyboard.hpp"
#include <glm/common.hpp>

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <chrono>
#include <memory>

Discord::RPC RPC{};

namespace Enforcer {

struct SimplePushConstantData {
  glm::mat4 transform{1.f}; // 1 0 0 1 // mat2{1.f} * v = v
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

  Camera camera{};
  const float FOV{50.f};
  const float FAR{100.f};
  // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, 0.f, 1.f));
  camera.SetViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = GameObject::CreateGameObject();
  Keyboard cameraController{};

  std::chrono::time_point currentTime{
      std::chrono::high_resolution_clock::now()};

  LOG_DEBUG(GLFW, "Loop started");
  LOG_DEBUG(Vulkan, "maxPushCostantsSize => {}",
            device.properties.limits.maxPushConstantsSize);

  while (!window.ShouldClose()) {
    glfwPollEvents();

    const std::chrono::time_point newTime{
        std::chrono::high_resolution_clock::now()};
    [[maybe_unused]] const float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    // frameTime =
    //     glm::min(frameTime, 10); // to make when holding the screen and
    //     pressing
    //  a button dont make the camera jump

    cameraController.MoveInPlaneXZ(window.GetGLFWWindow(), frameTime,
                                   viewerObject);
    camera.SetViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    const float aspect{renderer.GetAspectRatio()};
    // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.SetPerspectiveProjection(glm::radians(FOV), aspect, .1f, FAR);

    if (VkCommandBuffer commandBuffer = renderer.BeginFrame()) {
      renderer.BeginSwapChainRenderPass(commandBuffer);
      RenderGameObjects(commandBuffer, camera);
      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }
    RPC.Update(Discord::RPCStatus::Playing);
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}

std::unique_ptr<Model> createCubeModel(Device &device, glm::vec3 offset) {
  std::vector<Model::Vertex> vertices{

      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto &v : vertices) {
    v.position += offset;
  }
// temp
#ifdef DEBUG
  for (const Model::Vertex i : vertices) {
    LOG_TRACE(Vulkan, "Vertices => {}x:{}y:{}z", i.position.x, i.position.y,
              i.position.z);
  }
#endif

  return std::make_unique<Model>(device, vertices);
}

void Application::LoadGameObjects() {
  std::shared_ptr<Model> model = createCubeModel(device, {.0f, .0f, .0f});
  auto cube = GameObject::CreateGameObject();
  cube.model = model;
  cube.transform.translation = {0.f, 0.f, 2.5f};
  cube.transform.scale = {.5f, .5f, .5f};

  gameObjects.push_back(std::move(cube));
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

void Application::RenderGameObjects(VkCommandBuffer commandBuffer,
                                    const Camera &camera) {
  // atualized every frame

  pipeline->bind(commandBuffer);

  glm::mat4 projectionView = camera.GetProjection() * camera.GetView();

  for (GameObject &obj : gameObjects) {

    SimplePushConstantData push{};
    push.color = obj.color;
    push.transform = projectionView * obj.transform.mat4(); // temp

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);

    obj.model->Bind(commandBuffer);
    obj.model->Draw(commandBuffer);
  }
}
} // namespace Enforcer
