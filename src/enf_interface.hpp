#pragma once

#include <GLFW/glfw3.h>
#include "enf_descriptors.hpp"
#include "enf_renderer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_vulkan.h"
#include "../external/imgui/imgui.h"
#pragma GCC diagnostic pop

namespace Enforcer {
  class Interface {
  public:
    //Interface(GLFWwindow *window, Device &device, const Renderer &renderer, const DescriptorPool &pool) :
      //  window{window} {
    /*IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &IO = ImGui::GetIO();
    (void) IO;
    ImGui::Render();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForVulkan(window, true);

    initInfo.Instance = device.getInstance();
    initInfo.PhysicalDevice = device.getPhysicalDevice();
    initInfo.Device = device.device();
    initInfo.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily;
    initInfo.Queue = device.graphicsQueue();
    initInfo.DescriptorPool = pool->GetDescriptorPool();
    initInfo.RenderPass = renderer.GetSwapChainRenderPass();
    initInfo.MinImageCount = device.getSwapChainSupport().capabilities.minImageCount;
    initInfo.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    initInfo.PipelineCache = nullptr; // optional
    initInfo.Subpass = 0;

    initInfo.UseDynamicRendering = false; // optional

    initInfo.Allocator = nullptr; // optional
    initInfo.CheckVkResultFn = nullptr; // optional
    initInfo.MinAllocationSize = 0; // optional

    ImGui_ImplVulkan_Init(&initInfo);

    //VkCommandBuffer commandBuffer{device.beginSingleTimeCommands()};
    //ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    //device.endSingleTimeCommands(commandBuffer);
    //ImGui_ImplVulkan_DestroyFontUploadObjects();
  }
    static void CreateInterfaceWindow();

  private:
    ImGui_ImplVulkan_InitInfo initInfo{};
    GLFWwindow *window;*/
  };
} // namespace Enforcer
