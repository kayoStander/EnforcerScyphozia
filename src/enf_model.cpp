#include "enf_model.hpp"
#include "../common/assert.hpp"
#include "../common/utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../common/tiny_obj_loader.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <stdexcept>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace std {
template <> struct hash<Enforcer::Model::Vertex> {
  size_t operator()(Enforcer::Model::Vertex const &vertex) const {
    size_t seed{0};
    Enforcer::HashCombine(seed, vertex.position, vertex.color, vertex.normal,
                          vertex.uv);
    return seed;
  }
};
} // namespace std

namespace Enforcer {
Model::Model(Device &device, const Model::Data &data) : device{device} {
  CreateVertexBuffers(data.vertices);
  CreateIndexBuffers(data.indices);
}

Model::~Model() {
  vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
  vkFreeMemory(device.device(), vertexBufferMemory, nullptr);

  if (hasIndexBuffer) {
    vkDestroyBuffer(device.device(), indexBuffer, nullptr);
    vkFreeMemory(device.device(), indexBufferMemory, nullptr);
  }
}

void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<u32>(vertices.size());
  ASSERT_LOG(vertexCount >= 3, "Vertex count must be atleast 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer,
                      stagingBufferMemory); // host -> cpu / device -> gpu

  void *data;
  vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(device.device(), stagingBufferMemory);

  device.createBuffer(bufferSize,
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer,
                      vertexBufferMemory); // host -> cpu / device -> gpu

  device.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
  vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
}

void Model::CreateIndexBuffers(const std::vector<u32> &indices) {
  indexCount = static_cast<u32>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      stagingBuffer,
                      stagingBufferMemory); // host -> cpu / device -> gpu

  void *data;
  vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(device.device(), stagingBufferMemory);

  device.createBuffer(bufferSize,
                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer,
                      indexBufferMemory); // host -> cpu / device -> gpu

  device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

  vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
  vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
}

std::unique_ptr<Model> Model::CreateModelFromFile(Device &device,
                                                  const std::string &filepath) {
  Data data{};
  data.LoadModel(filepath);
  LOG_INFO(Vulkan, "Vertex count => {}", data.vertices.size());
  return std::make_unique<Model>(device, data);
}

void Model::Draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    return;
  }
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void Model::Bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[]{vertexBuffer};
  VkDeviceSize offsets[]{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  }
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

void Model::Data::LoadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        filepath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, u32> uniqueVertices{};
  for (const tinyobj::shape_t &shape : shapes) {
    for (const tinyobj::index_t &index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[static_cast<u32>(3 * index.vertex_index + 0)],
            attrib.vertices[static_cast<u32>(3 * index.vertex_index + 1)],
            attrib.vertices[static_cast<u32>(3 * index.vertex_index + 2)],
        };

        int colorIndex{3 * index.vertex_index + 2};
        if (colorIndex < static_cast<int>(attrib.colors.size())) {
          vertex.color = {
              attrib.colors[static_cast<u32>(colorIndex - 2)],
              attrib.colors[static_cast<u32>(colorIndex - 1)],
              attrib.colors[static_cast<u32>(colorIndex - 0)],
          };
        } else {
          vertex.color = {1.f, 1.f, 1.f};
        }
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[static_cast<u32>(3 * index.normal_index + 0)],
            attrib.normals[static_cast<u32>(3 * index.normal_index + 1)],
            attrib.normals[static_cast<u32>(3 * index.normal_index + 2)],
        };
      }

      if (index.normal_index >= 0) {
        vertex.uv = {
            attrib.texcoords[static_cast<u32>(3 * index.texcoord_index + 0)],
            attrib.texcoords[static_cast<u32>(3 * index.texcoord_index + 1)],
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<u32>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

} // namespace Enforcer
