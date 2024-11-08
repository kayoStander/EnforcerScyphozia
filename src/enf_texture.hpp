#pragma once

#include "enf_device.hpp"
#include <string>

namespace Enforcer {
class Texture {
public:
  Texture(Device &device, const std::string &filepath, u32 layerCount = 1);
  ~Texture();

  VkSampler GetSampler() const noexcept { return sampler; }
  VkImageView GetImageView() const noexcept { return imageView; }
  VkImageLayout GetImageLayout() const noexcept { return imageLayout; }

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(const Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;

  void SetImageFormat(const VkFormat format) noexcept { imageFormat = format; }

private:
  void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, u32 layerCount) const;
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
