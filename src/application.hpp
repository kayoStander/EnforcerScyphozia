#pragma once

#include "enf_device.hpp"
#include "enf_model.hpp"
#include "enf_pipeline.hpp"
#include "enf_swap_chain.hpp"
#include "enf_window.hpp"

#include <memory>
#include <vector>

namespace Enforcer {
class Application {
public:
  explicit Application();
  ~Application();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void Run();

private:
  void LoadModels();
  void CreatePipelineLayout();
  void CreatePipeline();
  void CreateCommandBuffers();
  void DrawFrame();

  Window window{};
  Device device{window};
  SwapChain swapChain{device, window.getExtent()};
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<Model> model;
};
} // namespace Enforcer
