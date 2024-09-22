#pragma once

#include "enf_device.hpp"
#include "enf_pipeline.hpp"
#include "enf_window.hpp"

#include "../common/config.hpp"

namespace Enforcer {
class Application {
public:
  void Run();

private:
  Window window{};
  Device device{window};
  Pipeline pipeline{device, "src/shaders/vertex.vert.glsl.spv",
                    "src/shaders/fragment.frag.glsl.spv",
                    Pipeline::DefaultPipelineConfigInfo(
                        Config::GetMainWindowGeometryWidth(),
                        Config::GetMainWindowGeometryHeight())};
};
} // namespace Enforcer
