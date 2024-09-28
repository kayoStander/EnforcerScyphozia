#pragma once

#include "../common/types.hpp"
#include "enf_device.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Enforcer {

struct PipelineConfigInfo {
  // PipelineConfigInfo(const PipelineConfigInfo&)=delete;
  // PipelineConfigInfo &operator=(const PipelineConfigInfo&)=delete;

  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  u32 subpass{0};
};

class Pipeline {
public:
  Pipeline(Device &device, const std::string &vertFilepath,
           const std::string &fragFilepath,
           const PipelineConfigInfo &ConfigInfo);
  ~Pipeline() noexcept;

  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  static PipelineConfigInfo DefaultPipelineConfigInfo(u32 width, u32 height);

private:
  static std::vector<char> ReadFile(const std::string &filepath);

  void CreateGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath,
                              const PipelineConfigInfo &configInfo);

  void CreateShaderModule(const std::vector<char> &code,
                          VkShaderModule *shaderModule);

  Device &device;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};
} // namespace Enforcer
