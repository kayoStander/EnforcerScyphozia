#include "application.hpp"
#include "../common/discord.hpp"
#include "enf_camera.hpp"
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

struct SimplePushConstantData {
  glm::mat4 transform{1.f}; // 1 0 0 1 // mat2{1.f} * v = v
  alignas(16) glm::vec3 color;
};

Application::Application() { LoadGameObjects(); }

Application::~Application() {}

void Application::Run() {
  RPC.Init();

  RenderSystem renderSystem{device, renderer.GetSwapChainRenderPass()};

  Camera camera{};
  const float FOV{50.f};
  const float FAR{100.f};
  // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(.5f, 0.f, 1.f));
  // camera.SetViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f,
  // 0.f, 2.5f));

  GameObject viewerObject = GameObject::CreateGameObject();
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
      renderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);
      renderer.EndSwapChainRenderPass(commandBuffer);
      renderer.EndFrame();
    }
    RPC.Update(Discord::RPCStatus::Playing);
  }

  vkDeviceWaitIdle(device.device());

  LOG_DEBUG(GLFW, "Loop ended");
}

void Application::LoadGameObjects() {
  std::shared_ptr<Model> coloredCube =
      Model::CreateModelFromFile(device, "model/colored_cube.obj");
  GameObject cube = GameObject::CreateGameObject();
  cube.model = coloredCube;
  cube.transform.translation = {.0f, .0f, 2.5f};
  cube.transform.scale = {.5f, .5f, .5f};

  std::shared_ptr<Model> smoothVase =
      Model::CreateModelFromFile(device, "model/smooth_vase.obj");
  GameObject vase = GameObject::CreateGameObject();
  vase.model = smoothVase;
  vase.transform.translation = {.0f, -.5f, 2.5f};
  vase.transform.scale = {3.f, 3.f, 3.f};

  gameObjects.push_back(std::move(cube));
  gameObjects.push_back(std::move(vase));
}

} // namespace Enforcer
