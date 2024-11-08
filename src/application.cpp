#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_buffer.hpp"
#include "enf_camera.hpp"
#include "enf_descriptors.hpp"
#include "enf_frame_info.hpp"
#include "enf_game_object.hpp"
#include "enf_model.hpp"
#include "enf_texture.hpp"
#include "keyboard.hpp"
#include "systems/enf_physics_system.hpp"
#include "systems/enf_point_light_system.hpp"
#include "systems/enf_render_system.hpp"
#include "systems/enf_skybox_system.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <chrono>
#include <random>

Discord::RPC RPC{};

// TODO: WINDOWS EXE
// TODO: IMPOSTERS WITH BILLBOARDS & BATCH RENDERING & OCCLUSION CULLING
// TODO: ImGui
// TODO: PHYSICS TOUCHING NOT WORKING YET
// TODO: BOIDS
// TODO: TRUE GRASS WITH TERRAIN
// TODO: SOUND

namespace Enforcer {

Application::Application() {

  defaultPool =
      DescriptorPool::Builder(device)
          .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                       SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                       MAX_TEXTURE * SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // cube
                       6 * SwapChain::MAX_FRAMES_IN_FLIGHT)
          .build();
  LoadGameObjects();
}

void Application::Run() {
  RPC.Init();

  std::vector<std::unique_ptr<Buffer>> uniformBufferObjectBuffers(
      SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (u32 i{0}; i < uniformBufferObjectBuffers.size(); ++i) {
    uniformBufferObjectBuffers[i] =
        std::make_unique<Buffer>(device, sizeof(DefaultUniformBufferObject), 1,
                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uniformBufferObjectBuffers[i]->map();
  }

  auto defaultSetLayout{
      DescriptorSetLayout::Builder(device)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURE)
          .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_FRAGMENT_BIT, 6) // cube
          .build()};

  AddTexture("image.jpg");
  AddTexture("image2.jpg");
  AddTexture("grass.jpg");
  AddTexture("yale.png");
  AddTexture("sky.jpg");

  AddTextureCube("sky.jpg");
  AddTextureCube("sky.jpg");
  AddTextureCube("sky.jpg");
  AddTextureCube("sky.jpg");
  AddTextureCube("sky.jpg");
  AddTextureCube("sky.jpg");

  std::vector<VkDescriptorImageInfo> imageInfos{textures.size()};

  for (u64 i{0}; i < textures.size(); ++i) {
    imageInfos[i].sampler = textures[i]->GetSampler();
    imageInfos[i].imageView = textures[i]->GetImageView();
    imageInfos[i].imageLayout = textures[i]->GetImageLayout();
  }

  std::vector<VkDescriptorImageInfo> cubeImageInfos{texturesCube.size()};

  for (u64 i{0}; i < texturesCube.size(); ++i) {
    cubeImageInfos[i].sampler = texturesCube[i]->GetSampler();
    cubeImageInfos[i].imageView = texturesCube[i]->GetImageView();
    cubeImageInfos[i].imageLayout = texturesCube[i]->GetImageLayout();
  }

  std::vector<VkDescriptorSet> defaultDescriptorSets(
      SwapChain::MAX_FRAMES_IN_FLIGHT);

  for (u32 i{0}; i < defaultDescriptorSets.size(); ++i) {
    VkDescriptorBufferInfo bufferInfo{
        uniformBufferObjectBuffers[i]->descriptorInfo()};
    DescriptorWriter(*defaultSetLayout, *defaultPool)
        .writeBuffer(0, &bufferInfo)
        .writeImageArray(1, imageInfos.data(), imageInfos.size())
        .writeImageCube(2, cubeImageInfos.data())
        .build(defaultDescriptorSets[i]);
  }

  RenderSystem renderSystem{device, renderer.GetSwapChainRenderPass(),
                            defaultSetLayout->getDescriptorSetLayout()};

  PointLightSystem pointLightSystem{device, renderer.GetSwapChainRenderPass(),
                                    defaultSetLayout->getDescriptorSetLayout()};

  SkyboxSystem skyboxSystem{device, renderer.GetSwapChainRenderPass(),
                            defaultSetLayout->getDescriptorSetLayout()};

  PhysicsSystem physicsSystem{};

  Camera camera{};
  constexpr float FOV{100.f};
  constexpr float FAR{75.f};
  // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, 0.f, 1.f));
  // camera.SetViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f,
  // 0.f, 2.5f));

  GameObject viewerObject{GameObject::CreateGameObject()};
  viewerObject.transform.translation = {.0f, -1.f, -2.5f};
  Keyboard cameraController{};

  std::chrono::time_point currentTime{
      std::chrono::high_resolution_clock::now()};

  LOG_DEBUG(GLFW, "Loop started");
  LOG_DEBUG(Vulkan, "maxPushConstantsSize => {}, UBO size => {}",
            device.properties.limits.maxPushConstantsSize,
            sizeof(DefaultUniformBufferObject));

  while (!window.ShouldClose()) {
    glfwPollEvents();

    const std::chrono::time_point newTime{
        std::chrono::high_resolution_clock::now()};
    const float frameTime{
        std::min(std::chrono::duration<float, std::chrono::seconds::period>(
                     newTime - currentTime)
                     .count(),
                 .1f)};
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
                          defaultDescriptorSets[static_cast<u32>(frameIndex)],
                          gameObjects};

      // update
      uniformBufferObject.projection = camera.GetProjection();
      uniformBufferObject.view = camera.GetView();
      uniformBufferObject.inverseView = camera.GetInverseView();

      physicsSystem.Update(frameInfo);
      pointLightSystem.Update(frameInfo, uniformBufferObject);
      // render
      renderer.BeginSwapChainRenderPass(commandBuffer);

      UpdateUniformBufferObject(uniformBufferObjectBuffers,
                                uniformBufferObject);

      // order between render matters
      skyboxSystem.RenderSkybox(frameInfo);
      renderSystem.RenderGameObjects(frameInfo);
      pointLightSystem.Render(frameInfo);

      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }

    RPC.Update(Discord::RPCStatus::Playing);

    std::cout << "\r\033[FFps => " << 1.f / frameTime << std::flush;
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}

void Application::LoadGameObjects() {

  auto fade = [](const float t) noexcept {
    return t * t * t * (t * (t * 6 - 15) + 10);
  };
  auto lerp = [](const float a, const float b, const float t) noexcept {
    return a + t * (b - a);
  };
  auto grad = [](const int hash, const float x, const float y) noexcept {
    switch (hash & 3) {
    case 0:
      return x + y;
    case 1:
      return x - y;
    case 2:
      return x + y;
    case 3:
      return -x + y;
    default:
      return 0.f;
    }
  };
  std::vector<int> perm(512);
  for (u32 i{0}; i < 256u; ++i) {
    perm[i] = static_cast<s32>(i);
  }
  std::shuffle(perm.begin(), perm.begin() + 256, std::mt19937(std::chrono::system_clock::now().time_since_epoch().count()));
  for (u32 i{0}; i < 256u; ++i) {
    perm[256u + i] = perm[i];
  }
  auto perlinNoise = [lerp,fade,grad](float x, float y, const std::vector<int> &perm,
                         const int octaves, float frequency, float amplitude) {
     float noiseValue{.0f};
     float maxAmplitude{.0f};

    for (int octave{0}; octave < octaves; ++octave) {
       auto freq = static_cast<float>(frequency * std::pow(2.0f, octave));
       auto amp = static_cast<float>(amplitude * std::pow(0.5f, octave));

      auto xi = static_cast<u32>(std::floor(x * freq)) & 255;
      auto yi = static_cast<u32>(std::floor(y * freq)) & 255;

      float xf = x * freq - std::floor(x * freq);
      float yf = y * freq - std::floor(y * freq);

      float u = fade(xf);
      float v = fade(yf);

      int aa = perm[xi + perm[yi]];
      int ab = perm[xi + perm[yi + 1u]];
      int ba = perm[xi + 1u + perm[yi]];
      int bb = perm[xi + 1u + perm[yi + 1u]];

      float x1 = lerp(grad(aa, xf, yf), grad(ab, xf, yf - 1), v);
      float x2 = lerp(grad(ba, xf - 1, yf), grad(bb, xf - 1, yf - 1), v);

      noiseValue += lerp(x1, x2, u) * amp;
      maxAmplitude += amp;
    }

    return noiseValue / maxAmplitude; // Normalize to [0, 1]
  };

  constexpr float frequency{.5f}, amplitude{5.f}, noiseScale{.5f};
  constexpr int octaves{4};

  const std::shared_ptr<Model> cube{
      Model::CreateModelFromFile(device, "model/cube.obj")};
  const std::shared_ptr<Model> imposterCube0{
      Model::CreateModelFromFile(device, "model/colored_cube.obj")};

  const std::shared_ptr<Model> smoothVase{
      Model::CreateModelFromFile(device, "model/smooth_vase.obj")};
  GameObject gameObject{GameObject::CreateGameObject()};
  gameObject.transform.scale = {.5f, .5f, .5f};
  gameObject.transform.translation = {0.f, -5.f, 0.f};
  gameObject.model = cube;
  gameObject.imageBind = 1;
  gameObject.reflection = 0.2f;
  // gameObject.physics.isStatic = true;
  gameObjects.emplace(gameObject.GetId(), std::move(gameObject));

  for (s32 x{-25}; x < 25; ++x) {
    for (s32 y{-25}; y < 25; ++y) {
      GameObject gameObjectf{GameObject::CreateGameObject()};

      const float noiseValue{
          perlinNoise(static_cast<float>(x) * noiseScale,
                      static_cast<float>(y) * noiseScale, perm, octaves,
                      frequency, amplitude),
      };

      gameObjectf.transform.scale = {.5f, .5f, .5f};
      gameObjectf.transform.translation = {static_cast<float>(x) * 1.f,
                                          noiseValue * 2.f,
                                          static_cast<float>(y) * 1.f};
      gameObjectf.model = cube;
      gameObjectf.imposters[0] = imposterCube0;
      gameObjectf.imageBind = 2;
      gameObjectf.reflection = 0.f;
      gameObjectf.physics.isStatic = true;

      gameObjects.emplace(gameObjectf.GetId(), std::move(gameObjectf));
    }
  }

  GameObject gameObject2{GameObject::CreateGameObject()};

  gameObject2.transform.scale = {5.f, 5.f, 5.f};
  gameObject2.transform.translation = {.0f, -.5f, .0f};
  gameObject2.model = smoothVase;
  gameObject2.imageBind = 4;
  gameObject2.reflection = 0.05f;
  gameObject2.physics.isStatic = true;

  gameObjects.emplace(gameObject2.GetId(), std::move(gameObject2));

  std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f}, {.1f, .1f, 1.f}, {.1f, 1.f, .1f},
      {1.f, 1.f, .1f}, {.1f, 1.f, 1.f}, {1.f, 1.f, 1.f} //
  };

  for (u32 i{0}; i < lightColors.size(); ++i) {
    GameObject pointLight{GameObject::MakePointLight(.45f)};
    pointLight.color = lightColors[i];
    glm::mat4 rotateLight{glm::rotate(
        glm::mat4{1.f}, (i * glm::two_pi<float>()) / static_cast<float>(lightColors.size()),
        {0.f, -1.f, 0.f})};
    pointLight.transform.translation =
        glm::vec3(rotateLight * glm::vec4{-1.f, -2.f, -1.f, 1.f});

    gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
  }
}

} // namespace Enforcer
