#include "enf_physics_system.hpp"
#include "../../common/logging/log.hpp"

namespace Enforcer {
bool CheckCollision(const GameObject &a, const GameObject &b) {
  const glm::vec3 aMin{a.transform.translation -
                       (a.model->GetBoundingBoxSize() * 0.5f)};
  const glm::vec3 aMax{a.transform.translation +
                       (a.model->GetBoundingBoxSize() * 0.5f)};
  const glm::vec3 bMin{b.transform.translation -
                       (b.model->GetBoundingBoxSize() * 0.5f)};
  const glm::vec3 bMax{b.transform.translation +
                       (b.model->GetBoundingBoxSize() * 0.5f)};

  return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
         (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
         (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

void PhysicsSystem::Update(FrameInfo &frameInfo) const {
  for (auto &a : frameInfo.gameObjects) {
    if (a.second.physics.isStatic || a.second.model == nullptr) [[unlikely]]
      continue;

    a.second.physics.ApplyForce(gravity * a.second.physics.mass);
    for (auto &b : frameInfo.gameObjects) {
      if (b.second.model == nullptr) [[unlikely]]
        continue;

      if (a.first != b.first) {
        if (CheckCollision(a.second, b.second)) {
          LOG_WARNING(Common, "Object{} collided with object{}",
                      a.second.GetId(), b.second.GetId());
          a.second.physics.velocity = glm::vec3(0.0f);
          b.second.physics.velocity *= .5f;
        }
      }
      a.second.physics.Update(frameInfo.frameTime, a.second.transform);
    }
  }
}
} // namespace Enforcer
