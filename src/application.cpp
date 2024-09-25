#include "application.hpp"
#include "../common/discord.hpp"
#include "../common/logging/log.hpp"
#include "enf_pipeline.hpp"
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Discord::RPC RPC{};

namespace Enforcer {
Application::Application() {
  CreatePipelineLayout();
  CreatePipeline();
  CreateCommandBuffers();
}

Application::~Application() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void Application::Run() {
  RPC.Init();

  LOG_DEBUG(GLFW, "Loop started");

  while (!window.ShouldClose()) {
    glfwPollEvents();
    RPC.Update(Discord::RPCStatus::Playing);
  }

  LOG_DEBUG(GLFW, "Loop ended");
}

void Application::CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;
  if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipelineLayout.");
  }
}

void Application::CreatePipeline() {
  PipelineConfigInfo pipelineConfig{Pipeline::DefaultPipelineConfigInfo(
      swapChain.width(), swapChain.height())};
  pipelineConfig.renderPass = swapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(
      device, "src/shaders/vertex.vert.glsl.spv",
      "src/shaders/fragment.frag.glsl.spv", pipelineConfig);
}

void Application::CreateCommandBuffers() {}
void Application::DrawFrame() {}

} // namespace Enforcer
