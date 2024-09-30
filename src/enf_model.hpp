#pragma once

#include "enf_device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#else
#error "No GLM recognized in the system"
#endif

namespace Enforcer {
class Model {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription>
    GetBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    GetAttributeDescriptions();
  };

  explicit Model(Device &device, const std::vector<Vertex> &vertices);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  void Bind(VkCommandBuffer commandBuffer);
  void Draw(VkCommandBuffer commandBuffer);

private:
  void CreateVertexBuffers(const std::vector<Vertex> &vertices);

  Device &device;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  u32 vertexCount;
};
} // namespace Enforcer
