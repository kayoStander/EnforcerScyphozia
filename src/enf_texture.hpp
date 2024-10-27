#pragma once

#include "enf_device.hpp"
#include <string>

namespace Enforcer {
class Texture {
public:
  Texture(Device &device, const std::string &filepath, u32 layerCount = 1);
  ~Texture();

  VkSampler GetSampler() { return sampler; }
  VkImageView GetImageView() { return imageView; }
  VkImageLayout GetImageLayout() { return imageLayout; }

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(const Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;

  void SetImageFormat(VkFormat format) noexcept { imageFormat = format; }

private:
  void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout,
                             u32 layerCount);
  void GenerateMipmaps();

  mutable s32 width{}, height{};
  Device &device;
  // order matters
  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;
  VkSampler sampler;
  VkFormat imageFormat;
  VkImageLayout imageLayout;
};
} // namespace Enforcer
