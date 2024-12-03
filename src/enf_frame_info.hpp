#pragma once

#include "enf_camera.hpp"
#include "enf_game_object.hpp"

#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace Enforcer {
  constexpr int MAX_LIGHTS{10};
  constexpr int MAX_TEXTURE{5};
  constexpr int OBJECT_PIXELIZATION{10};
  struct PointLight {
    glm::vec4 position{};
    glm::vec4 color{};
  };
  struct DefaultUniformBufferObject {
    // VkSampler image;
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
    glm::mat4 inverseView{1.f};
    glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
    PointLight pointLights[MAX_LIGHTS];
    float time{0.f};
    int numLights{};
  };
  struct FrameInfo {
    int frameIndex{};
    float frameTime{};
    VkCommandBuffer commandBuffer;
    Camera &camera;
    VkDescriptorSet globalDescriptorSet;
    std::unordered_map<u32, GameObject> &gameObjects;
  };
} // namespace Enforcer
