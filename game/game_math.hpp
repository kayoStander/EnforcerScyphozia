#pragma once

#include "../src/enf_game_object.hpp"


namespace Game::Math {
  constexpr float Distance(const glm::vec3 position1, const glm::vec3 position2) noexcept {
    return glm::length(position1 - position2);
  }
  constexpr glm::vec3 Lerp(const glm::vec3& start, const glm::vec3& end, const float deltaTime){
    return start + deltaTime * (end - start);
  }
}
