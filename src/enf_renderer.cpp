#include "enf_renderer.hpp"

#include <array>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Enforcer {

  Renderer::Renderer(Window &window, Device &device) : window{window}, device{device} {
    vkDeviceWaitIdle(device.device());

    LOG_DEBUG(Vulkan, "Renderer created");

    RecreateSwapChain();
    CreateCommandBuffers();
  }

  Renderer::~Renderer() {
    LOG_DEBUG(Vulkan, "Renderer destroyed");
    FreeCommandBuffers();
  }

  void Renderer::RecreateSwapChain() {
    LOG_INFO(Vulkan, "SwapChain recreated");
    vkDeviceWaitIdle(device.device());
    swapChain.reset();

    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = window.getExtent();
      vkDeviceWaitIdle(device.device());
      glfwWaitEvents();
    }
    vkDeviceWaitIdle(device.device());

    if (swapChain == nullptr) {
      swapChain = std::make_unique<SwapChain>(device, extent);
    } else {
      std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
      swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

      if (!oldSwapChain->CompareSwapFormats(*swapChain)) {
        throw std::runtime_error("swap chain image or depth format changed");
      }
    }

    // brb
  }

  void Renderer::CreateCommandBuffers() {
    commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<u32>(commandBuffers.size());

    VK_CHECK_RESULT_LOG(vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()),
                        "Failed to allocate command buffers!");
  }

  void Renderer::FreeCommandBuffers() {
    vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<u32>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
  }

  VkCommandBuffer Renderer::BeginFrame(/*u32 i*/) {
    ASSERT_LOG(!isFrameStarted, "Can't call BeginFrame() while alredy on progress");
    const auto result = swapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {

      vkDeviceWaitIdle(device.device());

      RecreateSwapChain();

      return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire the next swapchain image");
    }

    isFrameStarted = true;

    const VkCommandBuffer commandBuffer{GetCurrentCommandBuffer()};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK_RESULT_LOG(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

    return commandBuffer;
  }
  void Renderer::EndFrame() {
    ASSERT_LOG(isFrameStarted, "Can't call EndFrame() while frame is not in progress");

    const VkCommandBuffer commandBuffer{GetCurrentCommandBuffer()};

    VK_CHECK_RESULT_LOG(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer!");

    const auto result{swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex)};
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized()) {
      window.ResetWindowResizedFlag();
      RecreateSwapChain();

      isFrameStarted = false;

      return;
    }
    VK_CHECK_RESULT_LOG(result, "Failed to present swapchain image!");

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void Renderer::BeginSwapChainRenderPass(const VkCommandBuffer commandBuffer) const {
    ASSERT_LOG(isFrameStarted, "Can't call BeginSwapChainRenderPass() if frame is not in progress");
    ASSERT_LOG(commandBuffer == GetCurrentCommandBuffer(),
               "Can't begin render pass on command buffer in a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{.01f, .01f, .01f, 1.f}}; // BACKGROUND
    clearValues[1].depthStencil = {1.f, 0};
    renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<f32>(swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<f32>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }
  void Renderer::EndSwapChainRenderPass(const VkCommandBuffer commandBuffer) const {
    ASSERT_LOG(isFrameStarted, "Can't call EndSwapChainRenderPass() if frame is not in progress");
    ASSERT_LOG(commandBuffer == GetCurrentCommandBuffer(),
               "Can't end render pass on command buffer in a different frame");
    vkCmdEndRenderPass(commandBuffer);
  }

} // namespace Enforcer
