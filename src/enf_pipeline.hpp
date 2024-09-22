#pragma once

#include "../common/types.hpp"
#include "enf_device.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Enforcer {

struct PipelineConfigInfo {};

class Pipeline {
public:
  Pipeline(Device &device, const std::string &vertFilepath,
           const std::string &fragFilepath,
           const PipelineConfigInfo &ConfigInfo);
  ~Pipeline() noexcept {}

  Pipeline(const Pipeline &) = delete;
  void operator=(const Pipeline &) = delete;

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
