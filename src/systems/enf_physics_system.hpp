#pragma once
#include "../enf_frame_info.hpp"

namespace Enforcer {
class PhysicsSystem {
public:
  glm::vec3 gravity{.0f, 9.18f, .0f};
  void Update(const FrameInfo &frameInfo) const;
};
} // namespace Enforcer
