#pragma once

#include "enf_device.hpp"
#include <string>

namespace Enforcer {
class Texture {
public:
  Texture(Device &device, const std::string &filepath);
  ~Texture();

  VkSampler GetSampler() { return sampler; }
  VkImageView GetImageView() { return imageView; }
  VkImageLayout GetImageLayout() { return imageLayout; }

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(const Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;

private:
  void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
  Device &device;
  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;
  VkSampler sampler;
  VkFormat imageFormat;
  VkImageLayout imageLayout;
};
} // namespace Enforcer
