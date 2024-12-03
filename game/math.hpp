#pragma once

#include "../src/enf_game_object.hpp"

namespace Game {
  namespace Math {
    constexpr inline float Distance(const glm::vec3& position1, const glm::vec3& position2) noexcept {
      return glm::length(position1 - position2);
    }
  }
}