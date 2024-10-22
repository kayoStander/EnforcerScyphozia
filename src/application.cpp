#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_buffer.hpp"
#include "enf_camera.hpp"
#include "enf_descriptors.hpp"
#include "enf_game_object.hpp"
#include "enf_model.hpp"
#include "enf_texture.hpp"
#include "keyboard.hpp"
#include "systems/enf_point_light_system.hpp"
#include "systems/enf_render_system.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <numeric>
#include <ostream>
#include <ranges>

Discord::RPC RPC{};

namespace Enforcer {

Application::Application() {
  globalPool =
      DescriptorPool::Builder(device)
          .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                       SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                       2 * SwapChain::MAX_FRAMES_IN_FLIGHT) // 2 is texture amt
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

  auto globalSetLayout{
      DescriptorSetLayout::Builder(device)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_FRAGMENT_BIT, 2)
          .build()};

  textures.push_back(std::make_unique<Texture>(device, "textures/image.png"));
  textures.push_back(std::make_unique<Texture>(
      device, "textures/image3.png")); // not recognized for some reason

  std::vector<VkDescriptorImageInfo> imageInfos{{}, {}};

  for (u64 i{0}; i < textures.size(); ++i) {
    imageInfos[i].sampler = textures[i]->GetSampler();
    imageInfos[i].imageView = textures[i]->GetImageView();
    imageInfos[i].imageLayout = textures[i]->GetImageLayout();
  }

  std::vector<VkDescriptorSet> globalDescriptorSets(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (u32 i{0}; i < globalDescriptorSets.size(); ++i) {
    VkDescriptorBufferInfo bufferInfo{
        uniformBufferObjectBuffers[i]->descriptorInfo()};
    DescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImageArray(1, imageInfos.data(), imageInfos.size())
        .build(globalDescriptorSets[i]);
  }

  RenderSystem renderSystem{device, renderer.GetSwapChainRenderPass(),
                            globalSetLayout->getDescriptorSetLayout()};

  PointLightSystem pointLightSystem{device, renderer.GetSwapChainRenderPass(),
                                    globalSetLayout->getDescriptorSetLayout()};

  Camera camera{};
  const float FOV{100.f};
  const float FAR{100.f};
  // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, 0.f, 1.f));
  // camera.SetViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f,
  // 0.f, 2.5f));

  GameObject viewerObject{GameObject::CreateGameObject()};
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
    const float frameTime{
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count()};
    currentTime = newTime;

    cameraController.MoveInPlaneXZ(window.GetGLFWWindow(), frameTime,
                                   viewerObject);
    camera.SetViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    const float aspect{renderer.GetAspectRatio()};
    // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.SetPerspectiveProjection(glm::radians(FOV), aspect, .1f, FAR);

    if (VkCommandBuffer commandBuffer{renderer.BeginFrame()}) {
      const int frameIndex{static_cast<int>(renderer.GetFrameIndex())};
      FrameInfo frameInfo{frameIndex,
                          frameTime,
                          commandBuffer,
                          camera,
                          globalDescriptorSets[static_cast<u32>(frameIndex)],
                          gameObjects};
      // update
      GlobalUniformBufferObject uniformBufferObject{};
      uniformBufferObject.projection = camera.GetProjection();
      uniformBufferObject.view = camera.GetView();
      uniformBufferObject.inverseView = camera.GetInverseView();

      pointLightSystem.Update(frameInfo, uniformBufferObject);
      uniformBufferObjectBuffers[static_cast<u32>(frameIndex)]->writeToBuffer(
          &uniformBufferObject);
      uniformBufferObjectBuffers[static_cast<u32>(frameIndex)]->flush();

      // render
      renderer.BeginSwapChainRenderPass(commandBuffer);

      // order maters
      renderSystem.RenderGameObjects(frameInfo);
      pointLightSystem.Render(frameInfo);

      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }
    RPC.Update(Discord::RPCStatus::Playing);

    std::cout << "\r\033[KFPS => " << 1.f / frameTime << std::flush;
    // LOG_WARNING(Core, "\r\033[KFPS => {}", 1.f / frameTime);
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}

void Application::LoadGameObjects() {
  std::shared_ptr<Texture> simpleTexture{std::make_shared<Texture>(device, "")};

  std::shared_ptr<Model> coloredCubeModel{Model::CreateModelFromFile(
      device, "model/smooth_vase.obj", simpleTexture)};
  GameObject cube = GameObject::CreateGameObject();
  cube.model = coloredCubeModel;
  cube.transform.translation = {-1.0f, .0f, 0.f};
  cube.transform.scale = {3.f, 3.f, 3.f};

  std::shared_ptr<Model> smoothVaseModel{
      Model::CreateModelFromFile(device, "model/flat_vase.obj", simpleTexture)};
  GameObject vase = GameObject::CreateGameObject();
  vase.model = smoothVaseModel;
  vase.transform.translation = {1.0f, .0f, 0.f};
  vase.transform.scale = {3.f, 3.f, 3.f};

  std::shared_ptr<Model> quadModel{
      Model::CreateModelFromFile(device, "model/quad.obj", simpleTexture)};
  GameObject quad = GameObject::CreateGameObject();
  quad.model = quadModel;
  quad.transform.translation = {.0f, .0f, 0.f};
  quad.transform.scale = {10.f, 10.f, 10.f};

  std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f}, {.1f, .1f, 1.f}, {.1f, 1.f, .1f},
      {1.f, 1.f, .1f}, {.1f, 1.f, 1.f}, {1.f, 1.f, 1.f} //
  };

  for (u32 i{0}; i < lightColors.size(); ++i) {
    GameObject pointLight{GameObject::MakePointLight(.45f)};
    pointLight.color = lightColors[i];
    glm::mat4 rotateLight{glm::rotate(
        glm::mat4{1.f}, (i * glm::two_pi<float>()) / lightColors.size(),
        {0.f, -1.f, 0.f})};
    pointLight.transform.translation =
        glm::vec3(rotateLight * glm::vec4{-1.f, -2.f, -1.f, 1.f});

    gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
  }

  gameObjects.emplace(cube.GetId(), std::move(cube));
  gameObjects.emplace(vase.GetId(), std::move(vase));
  gameObjects.emplace(quad.GetId(), std::move(quad));
}

} // namespace Enforcer
