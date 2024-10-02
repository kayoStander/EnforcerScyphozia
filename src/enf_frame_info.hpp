#pragma once

#include "enf_camera.hpp"
#include "enf_game_object.hpp"

#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace Enforcer {
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  Camera &camera;
  VkDescriptorSet globalDescriptorSet;
  std::unordered_map<u32, GameObject> &gameObjects;
};
} // namespace Enforcer
