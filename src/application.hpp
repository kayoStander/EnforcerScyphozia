#pragma once

#include "enf_device.hpp"
#include "enf_model.hpp"
#include "enf_pipeline.hpp"
#include "enf_swap_chain.hpp"
#include "enf_window.hpp"

#include <memory>
#include <vector>

#define WIDTH 800
#define HEIGHT 600

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
  void RecreateSwapChain();
  void RecordCommandBuffer(u32 imageIndex);

  Window window{WIDTH, HEIGHT};
  Device device{window};
  std::unique_ptr<SwapChain> swapChain;
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<Model> model;
};
} // namespace Enforcer
