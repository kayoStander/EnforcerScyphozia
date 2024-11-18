#include "enf_physics_system.hpp"
#include "../../common/logging/log.hpp"

namespace Enforcer {
  bool CheckCollision(const GameObject &a, const GameObject &b) {
    const glm::vec3 delta = a.transform.translation - b.transform.translation;
    if (constexpr float maxDistanceSquared = 10.0f * 10.0f; dot(delta, delta) > maxDistanceSquared) {
      return false;
    }

    const glm::vec3 aMin{a.transform.translation - (a.model->GetBoundingBoxSize() * a.transform.scale)};
    const glm::vec3 aMax{a.transform.translation + (a.model->GetBoundingBoxSize() * a.transform.scale)};
    const glm::vec3 bMin{b.transform.translation - (b.model->GetBoundingBoxSize() * b.transform.scale)};
    const glm::vec3 bMax{b.transform.translation + (b.model->GetBoundingBoxSize() * b.transform.scale)};

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
  }

  void PhysicsSystem::Update(const FrameInfo &frameInfo) const {
    // more objs = mode speed, bad way to implement
    for (auto &[keyA, objA]: frameInfo.gameObjects) {
      if (objA.physics.isStatic || objA.model == nullptr) [[unlikely]]
        continue;

      objA.physics.ApplyForce(gravity * objA.physics.mass);
      objA.physics.Update(frameInfo.frameTime, objA.transform);

      for (auto &[keyB, objB]: frameInfo.gameObjects) {
        if (objB.model == nullptr || keyA == keyB) [[unlikely]]
          continue;

        if (CheckCollision(objA, objB)) {
        LOG_WARNING(Common, "Object{} collided with object{}", objA.GetId(),
                 objB.GetId());
          const glm::vec3 bMin{objB.transform.translation -
                                                (objB.model->GetBoundingBoxSize() * objB.transform.scale)};
          const glm::vec3 aMin{objA.transform.translation -
                                                (objA.model->GetBoundingBoxSize() * objA.transform.scale)};

          /*LOG_WARNING(
              Debug, "Positions: A({} {} {}) B({} {} {})",
              objA.transform.translation.x, objA.transform.translation.y,
              objA.transform.translation.z, objB.transform.translation.x,
              objB.transform.translation.y, objB.transform.translation.z);
          LOG_WARNING(Debug, "Bounding Box A({} {} {}) B({} {} {})", aMin.x,
                      aMin.y, aMin.z, bMin.x, bMin.y, bMin.z);
*/
          objA.physics.velocity = glm::vec3{0.0f};
        }
      }
    }
  }
} // namespace Enforcer
