#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_buffer.hpp"
#include "enf_camera.hpp"
#include "enf_descriptors.hpp"
#include "enf_game_object.hpp"
#include "enf_model.hpp"
#include "enf_render_system.hpp"
#include "keyboard.hpp"

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

struct GlobalUniformBufferObject {
  glm::mat4 projectionView{1.f};
  glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
  glm::vec3 lightPosition{-1.f}; // ignore w
  alignas(16) glm::vec4 lightColor{1.f};
};

Application::Application() {
  globalPool = DescriptorPool::Builder(device)
                   .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                SwapChain::MAX_FRAMES_IN_FLIGHT)
                   .build();
  LoadGameObjects();
}

Application::~Application() {}

void Application::Run() {
  RPC.Init();

  std::vector<std::unique_ptr<Buffer>> uniformBufferObjectBuffers(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (u32 i{0}; i < uniformBufferObjectBuffers.size(); ++i) {
    uniformBufferObjectBuffers[i] =
        std::make_unique<Buffer>(device, sizeof(GlobalUniformBufferObject), 1,
                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uniformBufferObjectBuffers[i]->map();
  }

  auto globalSetLayout{DescriptorSetLayout::Builder(device)
                           .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       VK_SHADER_STAGE_ALL_GRAPHICS)
                           .build()};

  std::vector<VkDescriptorSet> globalDescriptorSets(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (u32 i{0}; i < globalDescriptorSets.size(); ++i) {
    auto bufferInfo{uniformBufferObjectBuffers[i]->descriptorInfo()};
    DescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }

  RenderSystem renderSystem{device, renderer.GetSwapChainRenderPass(),
                            globalSetLayout->getDescriptorSetLayout()};

  Camera camera{};
  const float FOV{100.f};
  const float FAR{100.f};
  // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, 0.f, 1.f));
  // camera.SetViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f,
  // 0.f, 2.5f));

  GameObject viewerObject = GameObject::CreateGameObject();
  viewerObject.transform.translation = {.0f, -1.f, -2.5f};
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
    const float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.MoveInPlaneXZ(window.GetGLFWWindow(), frameTime,
                                   viewerObject);
    camera.SetViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    const float aspect{renderer.GetAspectRatio()};
    // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.SetPerspectiveProjection(glm::radians(FOV), aspect, .1f, FAR);

    if (VkCommandBuffer commandBuffer = renderer.BeginFrame()) {
      const int frameIndex{static_cast<int>(renderer.GetFrameIndex())};
      FrameInfo frameInfo{frameIndex,
                          frameTime,
                          commandBuffer,
                          camera,
                          globalDescriptorSets[static_cast<u32>(frameIndex)],
                          gameObjects};
      // update
      GlobalUniformBufferObject uniformBufferObject{};
      uniformBufferObject.projectionView =
          camera.GetProjection() * camera.GetView();
      uniformBufferObjectBuffers[static_cast<u32>(frameIndex)]->writeToBuffer(
          &uniformBufferObject);
      uniformBufferObjectBuffers[static_cast<u32>(frameIndex)]->flush();

      // render
      renderer.BeginSwapChainRenderPass(commandBuffer);
      renderSystem.RenderGameObjects(frameInfo);
      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }
    RPC.Update(Discord::RPCStatus::Playing);
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}

void Application::LoadGameObjects() {
  std::shared_ptr<Model> coloredCubeModel =
      Model::CreateModelFromFile(device, "model/smooth_vase.obj");
  GameObject cube = GameObject::CreateGameObject();
  cube.model = coloredCubeModel;
  cube.transform.translation = {-1.0f, .0f, 0.f};
  cube.transform.scale = {3.f, 3.f, 3.f};

  std::shared_ptr<Model> smoothVaseModel =
      Model::CreateModelFromFile(device, "model/flat_vase.obj");
  GameObject vase = GameObject::CreateGameObject();
  vase.model = smoothVaseModel;
  vase.transform.translation = {1.0f, .0f, 0.f};
  vase.transform.scale = {3.f, 3.f, 3.f};

  std::shared_ptr<Model> quadModel =
      Model::CreateModelFromFile(device, "model/quad.obj");
  GameObject quad = GameObject::CreateGameObject();
  quad.model = quadModel;
  quad.transform.translation = {.0f, .0f, 0.f};
  quad.transform.scale = {10.f, 10.f, 10.f};

  gameObjects.emplace(cube.GetId(), std::move(cube));
  gameObjects.emplace(vase.GetId(), std::move(vase));
  gameObjects.emplace(quad.GetId(), std::move(quad));
}

} // namespace Enforcer
