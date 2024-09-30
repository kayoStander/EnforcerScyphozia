#pragma once

#include "../common/assert.hpp"
#include "enf_device.hpp"
#include "enf_swap_chain.hpp"
#include "enf_window.hpp"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Enforcer {
class Renderer {
public:
  explicit Renderer(Window &window, Device &device);
  ~Renderer();

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  VkCommandBuffer BeginFrame();
  void EndFrame();
  void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

  VkRenderPass GetSwapChainRenderPass() const {
    return swapChain->getRenderPass();
  }
  bool IsFrameInProgress() const noexcept { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    ASSERT_LOG(isFrameStarted,
               "Cannot get command buffer while frame not in progress");
    return commandBuffers[static_cast<u32>(currentFrameIndex)];
  }

  int GetFrameIndex() const {
    ASSERT_LOG(isFrameStarted,
               "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

private:
  void CreateCommandBuffers();
  void FreeCommandBuffers();
  void RecreateSwapChain();

  Window &window;
  Device &device;
  std::unique_ptr<SwapChain> swapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  u32 currentImageIndex;
  int currentFrameIndex;
  bool isFrameStarted{false};
};
} // namespace Enforcer
