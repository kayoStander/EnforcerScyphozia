#include "enf_pipeline.hpp"
#include "../common/logging/log.hpp"

#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Enforcer {
std::vector<char> Pipeline::ReadFile(const std::string &filepath) {
  std::ifstream file{filepath, std::ios::ate | std::ios::binary};

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filepath);
  }

  size_t fileSize{static_cast<size_t>(file.tellg())};

  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

  file.close();
  return buffer;
}

void Pipeline::CreateGraphicsPipeline(
    const std::string &vertFilepath, const std::string &fragFilepath,
    [[maybe_unused]] const PipelineConfigInfo &configInfo) {
  auto vertexCode{ReadFile(vertFilepath)};
  auto fragmentCode{ReadFile(fragFilepath)};

  LOG_INFO(Shader, "[Vertex:Fragment] => [{}:{}]", vertexCode.size(),
           fragmentCode.size());
}

void Pipeline::CreateShaderModule(const std::vector<char> &code,
                                  VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const u32 *>(code.data());

  if (vkCreateShaderModule(device.device(), &createInfo, nullptr,
                           shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create shader module");
  }
}

PipelineConfigInfo
Pipeline::DefaultPipelineConfigInfo([[maybe_unused]] u32 width,
                                    [[maybe_unused]] u32 height) {
  PipelineConfigInfo configInfo{};

  return configInfo;
}

Pipeline::Pipeline(Device &device, const std::string &vertFilepath,
                   const std::string &fragFilepath,
                   const PipelineConfigInfo &configInfo)
    : device{device} {
  CreateGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
}

} // namespace Enforcer
