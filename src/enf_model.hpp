#pragma once

#include "enf_device.hpp"
#include <memory>
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
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription>
    GetBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    GetAttributeDescriptions();

    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color &&
             normal == other.normal && uv == other.uv;
    }
  };

  struct Data {
    std::vector<Vertex> vertices{};
    std::vector<u32> indices{};

    void LoadModel(const std::string &filepath);
  };

  explicit Model(Device &device, const Model::Data &data);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  static std::unique_ptr<Model>
  CreateModelFromFile(Device &device, const std::string &filepath);

  void Bind(VkCommandBuffer commandBuffer);
  void Draw(VkCommandBuffer commandBuffer);

private:
  void CreateVertexBuffers(const std::vector<Vertex> &vertices);
  void CreateIndexBuffers(const std::vector<u32> &indices);

  Device &device;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  u32 vertexCount;

  bool hasIndexBuffer{false};
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  u32 indexCount;
};
} // namespace Enforcer