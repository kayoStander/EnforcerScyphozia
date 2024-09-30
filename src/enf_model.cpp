#include "enf_model.hpp"
#include "../common/assert.hpp"

#include <vulkan/vulkan_core.h>

namespace Enforcer {
Model::Model(Device &device, const std::vector<Vertex> &vertices)
    : device{device} {
  CreateVertexBuffers(vertices);
}

Model::~Model() {
  vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
  vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
}

void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<u32>(vertices.size());
  ASSERT_LOG(vertexCount >= 3, "Vertex count must be atleast 3");

  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      vertexBuffer,
                      vertexBufferMemory); // host -> cpu / device -> gpu
  void *data;
  vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(device.device(), vertexBufferMemory);
}

void Model::Draw(VkCommandBuffer commandBuffer) {
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void Model::Bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[]{vertexBuffer};
  VkDeviceSize offsets[]{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

std::vector<VkVertexInputBindingDescription>
Model::Vertex::GetBindingDescriptions() {
  /*std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
*/
  return {{
      .binding = 0,
      .stride = sizeof(Vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  }};
}

std::vector<VkVertexInputAttributeDescription>
Model::Vertex::GetAttributeDescriptions() {
  /*std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = 0;*/
  return {{.location = 0,
           .binding = 0,
           .format = VK_FORMAT_R32G32B32_SFLOAT,
           .offset = offsetof(Vertex, position)},
          {.location = 1,
           .binding = 0,
           .format = VK_FORMAT_R32G32B32_SFLOAT,
           .offset = offsetof(Vertex, color)}};
}

} // namespace Enforcer
