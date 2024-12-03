#include "enf_physics_system.hpp"
#include "../../common/logging/log.hpp"

namespace Enforcer {
  bool CheckCollision(const GameObject &a, const GameObject &b) {
    const glm::vec3 delta{a.transform.translation - b.transform.translation};
    const float aDiagonal{length(a.model->GetBoundingBoxSize() * a.transform.scale)};
    const float bDiagonal{length(b.model->GetBoundingBoxSize() * b.transform.scale)};
    constexpr float bufferDistance{5.f};

    if (const float maxAllowedDistance{aDiagonal + bDiagonal + bufferDistance};
        dot(delta, delta) > maxAllowedDistance * maxAllowedDistance) {
      return false;
    }

    const glm::vec3 aMin{a.transform.translation - (a.model->GetBoundingBoxSize() * a.transform.scale * .5f)};
    const glm::vec3 aMax{a.transform.translation + (a.model->GetBoundingBoxSize() * a.transform.scale * .5f)};
    const glm::vec3 bMin{b.transform.translation - (b.model->GetBoundingBoxSize() * b.transform.scale * .5f)};
    const glm::vec3 bMax{b.transform.translation + (b.model->GetBoundingBoxSize() * b.transform.scale * .5f)};

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
  }

  void PhysicsSystem::Update(const FrameInfo &frameInfo) const {
    for (auto &[keyA, objA]: frameInfo.gameObjects) {
      if (objA.physics.isStatic or objA.model == nullptr)
        continue;

      if (!objA.physics.isGrounded) {
        objA.physics.ApplyForce(gravity * objA.physics.mass);
      }

      objA.physics.Update(frameInfo.frameTime, objA.transform);

      const GameObject *lastCollidedObject{nullptr};

      for (auto &[keyB, objB]: frameInfo.gameObjects) {
        if (objB.model == nullptr or keyA == keyB)
          continue;

        if (CheckCollision(objA, objB)) {
          // objA.physics.ApplyBounce(objA, objB, frameInfo.frameTime);
          objA.physics.isGrounded = true;
          objA.physics.velocity = glm::vec3{0.0f};

          const glm::vec3 contactPoint{.5f * (objA.transform.translation + objB.transform.translation)};
          const glm::vec3 offset{contactPoint - objA.transform.translation};
          const float torque{length(cross(offset,normalize(offset)))};

          objA.physics.torque += torque;
          lastCollidedObject = &objB;

          // LOG_INFO(Debug,"Torque => {}", objA.physics.torque);
          // LOG_INFO(Debug,"Angular Velocity => {}", objA.physics.angularVelocity);
          // LOG_INFO(Debug,"Rotation => ({},{},{})", objA.transform.rotation.x,objA.transform.rotation.y,objA.transform.rotation.z);

        } else {

          if (!objA.physics.isGrounded or lastCollidedObject == nullptr) {
            continue;
          }

          if (!CheckCollision(objA, *lastCollidedObject)) {
            objA.physics.isGrounded = false;
            lastCollidedObject = nullptr;
          }
        }

        //objA.physics.angularVelocity *= 0.98f; // damping if I wish so
      }
    }
  }
} // namespace Enforcer
