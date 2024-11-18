#include "enf_raycast.hpp"
#include <vector>
namespace Enforcer {
  bool RaycastIntersect(const glm::vec3 origin, const glm::vec3 direction, const glm::vec3 min, const glm::vec3 max,
                        glm::vec3 &intersectionPoint) {
    glm::vec3 tMin{(min - origin) / direction};
    glm::vec3 tMax{(max - origin) / direction};
    if (direction.x < 0.f)
      std::swap(tMin.x, tMax.x);
    if (direction.y < 0.f)
      std::swap(tMin.y, tMax.y);
    if (direction.z < 0.f)
      std::swap(tMin.z, tMax.z);
    const float near{std::max(std::max(tMin.x, tMax.y), tMin.z)};

    if (const float far{std::min(std::min(tMax.x, tMax.y), tMax.z)}; near > far or far < 0.f) {
      return false;
    }

    intersectionPoint = origin + direction * near;
    return true;
  }
  bool Raycast([[maybe_unused]] Ray &ray) { return true; }
} // namespace Enforcer
