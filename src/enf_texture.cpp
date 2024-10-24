#include "enf_texture.hpp"
#include "../common/assert.hpp"
#include "../common/logging/log.hpp"
#include "enf_buffer.hpp"
// #include <algorithm>
//  #include <span>
#include <vulkan/vulkan_core.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.hpp"
#pragma GCC diagnostic pop
#include <stdexcept>

namespace Enforcer {
Texture::Texture(Device &device, const std::string &filepath) : device{device} {
  if (filepath.empty()) {
    LOG_WARNING(Vulkan, "Texture {}loaded as empty so it will be ignored",
                filepath);
    return;
  }
  int bytesPerPixel{};
  [[maybe_unused]] int channels{};

  stbi_uc *data =
      stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, 4);
  ASSERT_LOG(data, "Failed to load texture {}", filepath);
  LOG_INFO(Vulkan, "Loaded texture {} (W:{},H:{},CH:{},BPP:{})", filepath,
           width, height, channels, bytesPerPixel);

  Buffer stagingBuffer{device, 4, static_cast<u32>(width * height),
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
  stagingBuffer.map();
  stagingBuffer.writeToBuffer(data);

  imageFormat = VK_FORMAT_R8G8B8A8_SRGB; /* SNORM inverts ig?*/

  vkDeviceWaitIdle(device.device());

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = imageFormat;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.extent = {static_cast<u32>(width), static_cast<u32>(height), 1};
  imageInfo.usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             image, imageMemory);

  TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  device.copyBufferToImage(stagingBuffer.getBuffer(), image,
                           static_cast<u32>(width), static_cast<u32>(height),
                           1);

  TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_NEAREST;
  samplerInfo.minFilter = VK_FILTER_NEAREST;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.mipLodBias = 0.f;
  samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
  samplerInfo.minLod = 0.f;
  samplerInfo.maxLod = 0.f;
  samplerInfo.maxAnisotropy = 1.f;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

  vkCreateSampler(device.device(), &samplerInfo, nullptr, &sampler);

  VkImageViewCreateInfo imageViewInfo{};
  imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D /*_ARRAY*/;
  imageViewInfo.format = imageFormat;
  imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                              VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = 1;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.image = image;

  vkCreateImageView(device.device(), &imageViewInfo, nullptr, &imageView);

  stbi_image_free(data);
}

Texture::~Texture() {
  vkDestroyImage(device.device(), image, nullptr);
  vkFreeMemory(device.device(), imageMemory, nullptr);
  vkDestroyImageView(device.device(), imageView, nullptr);
  vkDestroySampler(device.device(), sampler, nullptr);
}

void Texture::TransitionImageLayout(VkImageLayout oldLayout,
                                    VkImageLayout newLayout) {
  VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::runtime_error("unsupported layout transition");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
  vkDeviceWaitIdle(device.device());
  device.endSingleTimeCommands(commandBuffer);
}
} // namespace Enforcer
