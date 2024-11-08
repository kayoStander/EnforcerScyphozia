#include "enf_model.hpp"
#include "../common/assert.hpp"
#include "../common/utils.hpp"
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../external/tiny_obj_loader.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#else
#error "No GLM recognized in the system"
#endif

#define ROOT ""

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
  ComputeBoundingBox(data);
}

Model::~Model() {}

void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<u32>(vertices.size());
  ASSERT_LOG(vertexCount >= 3, "Vertex count must be atleast 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  u32 vertexSize = sizeof(vertices[0]);

  Buffer stagingBuffer{device, vertexSize, vertexCount,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)vertices.data());

  vertexBuffer = std::make_unique<Buffer>(device, vertexSize, vertexCount,
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(),
                    bufferSize);
}

void Model::CreateIndexBuffers(const std::vector<u32> &indices) {
  indexCount = static_cast<u32>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  u32 indexSize = sizeof(indices[0]);

  Buffer stagingBuffer{device, indexSize, indexCount,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)indices.data());

  indexBuffer = std::make_unique<Buffer>(device, indexSize, indexCount,
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(),
                    bufferSize);
}

std::unique_ptr<Model>
Model::CreateModelFromFile(Device &device, const std::string &modelFilepath) {
  Data data{};
  data.LoadModel(modelFilepath);
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
  VkBuffer buffers[]{vertexBuffer->getBuffer()};
  VkDeviceSize offsets[]{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0,
                         VK_INDEX_TYPE_UINT32);
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
           .offset = offsetof(Vertex, color)},
          {.location = 2,
           .binding = 0,
           .format = VK_FORMAT_R32G32B32_SFLOAT,
           .offset = offsetof(Vertex, normal)},
          {.location = 3,
           .binding = 0,
           .format = VK_FORMAT_R32G32_SFLOAT,
           .offset = offsetof(Vertex, uv)}};
}

void Model::Data::LoadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        (ROOT + filepath).c_str())) {
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

        vertex.color = {
            attrib.colors[static_cast<u32>(3 * index.vertex_index + 0)],
            attrib.colors[static_cast<u32>(3 * index.vertex_index + 1)],
            attrib.colors[static_cast<u32>(3 * index.vertex_index + 2)],
        };
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
            attrib.texcoords[static_cast<u32>(2 * index.texcoord_index + 0)],
            attrib.texcoords[static_cast<u32>(2 * index.texcoord_index + 1)],
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

void Model::ComputeBoundingBox(const Data data) {
  if (data.vertices.empty()) {
    LOG_WARNING(Vulkan, "No vertices in obj to make an bounding box!");
    return;
  }
  for (const Vertex &vertex : data.vertices) {
    boundingBoxMin = glm::min(boundingBoxMin, vertex.position);
    boundingBoxMax = glm::max(boundingBoxMax, vertex.position);
  }
  ASSERT_LOG(boundingBoxMin != glm::vec3(FLT_MAX) ||
                 boundingBoxMax != glm::vec3(-FLT_MAX),
             "Bouding box could not be computed correctly!");

  ASSERT_LOG(boundingBoxMin.x < boundingBoxMax.x &&
                 boundingBoxMin.y < boundingBoxMax.y &&
                 boundingBoxMin.z < boundingBoxMax.z,
             "Bounding box could not be computed correctly!");
}

} // namespace Enforcer
