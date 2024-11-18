#pragma once

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#else
#error "No GLM recognized in the system"
#endif

namespace Enforcer {
  class Ray {
  public:
    Ray(const glm::vec3 origin, const glm::vec3 direction) : origin{origin}, direction{direction} {}
    glm::vec3 origin;
    glm::vec3 direction;

  private:
    glm::vec3 boundMin{FLT_MAX};
    glm::vec3 boundMax{-FLT_MAX};
  };
  bool Raycast(Ray);
} // namespace Enforcer
