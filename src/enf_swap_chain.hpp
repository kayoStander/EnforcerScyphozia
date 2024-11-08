#pragma once

#include <assert.hpp>

#include "enf_device.hpp"

#include <memory>
#include <vulkan/vulkan.h>

namespace Enforcer {

class SwapChain {
public:
  static constexpr int MAX_FRAMES_IN_FLIGHT{2};

  SwapChain(Device &deviceRef, VkExtent2D windowExtent);
  SwapChain(Device &deviceRef, VkExtent2D windowExtent,
            const std::shared_ptr<SwapChain> &previous);

  ~SwapChain();

  SwapChain(const SwapChain &) = delete;
  SwapChain &operator=(const SwapChain &) = delete;

  [[nodiscard]] VkFramebuffer getFrameBuffer(const size_t index) const noexcept {
    return swapChainFramebuffers[index];
  }
  [[nodiscard]] VkRenderPass getRenderPass() const { return renderPass; }
  [[nodiscard]] VkImageView getImageView(const size_t index) const noexcept {
    return swapChainImageViews[index];
  }
  [[nodiscard]] size_t imageCount() const noexcept { return swapChainImages.size(); }
  [[nodiscard]] VkFormat getSwapChainImageFormat() const noexcept { return swapChainImageFormat; }
  [[nodiscard]] VkExtent2D getSwapChainExtent() const noexcept { return swapChainExtent; }
  [[nodiscard]] u32 width() const noexcept { return swapChainExtent.width; }
  [[nodiscard]] u32 height() const noexcept { return swapChainExtent.height; }

  [[nodiscard]] float extentAspectRatio() const {
    ASSERT_LOG(swapChainExtent.width != 0 || swapChainExtent.height != 0, "Cant divide swapChains width/height by 0");
    return static_cast<float>(swapChainExtent.width) /
           static_cast<float>(swapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex) const;
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers,
                                uint32_t const *imageIndex);

  [[nodiscard]] bool CompareSwapFormats(const SwapChain &swapChain) const noexcept {
    return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
           swapChain.swapChainImageFormat == swapChainImageFormat;
  }

private:
  void Init();
  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createRenderPass();
  void createFrameBuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat swapChainImageFormat;
  VkFormat swapChainDepthFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  Device &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  std::shared_ptr<SwapChain> oldSwapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame{0};
};

} // namespace Enforcer
