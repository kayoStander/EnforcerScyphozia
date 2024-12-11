#include "enf_game_object.hpp"
namespace Enforcer {
  glm::mat4 TransformComponent::mat4() const noexcept {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{{
                     scale.x * (c1 * c3 + s1 * s2 * s3),
                     scale.x * (c2 * s3),
                     scale.x * (c1 * s2 * s3 - c3 * s1),
                     0.0f,
                     },
                     {
                     scale.y * (c3 * s1 * s2 - c1 * s3),
                     scale.y * (c2 * c3),
                     scale.y * (c1 * c3 * s2 + s1 * s3),
                     0.0f,
                     },
                     {
                     scale.z * (c2 * s1),
                     scale.z * (-s2),
                     scale.z * (c1 * c2),
                     0.0f,
                     },
                     {translation.x, translation.y, translation.z, 1.0f}};
  }

  GameObject GameObject::Clone() const {
    GameObject clone{CreateGameObject()};
    clone.color = this->color;
    clone.transform = this->transform;
    clone.physics = this->physics;

    clone.model = this->model;
    clone.imposters = this->imposters;
    if (this->pointLight) {
      clone.pointLight = std::make_unique<PointLightComponent>(*this->pointLight);
    }

    clone.reflection = this->reflection;
    clone.imageBindRepeatFactor = this->imageBindRepeatFactor;
    clone.imageBind = this->imageBind;

    return clone;
  }

  glm::mat3 TransformComponent::normalMatrix() const noexcept {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 inverseScale{1.f / scale};

    return glm::mat3{{
                     inverseScale.x * (c1 * c3 + s1 * s2 * s3),
                     inverseScale.x * (c2 * s3),
                     inverseScale.x * (c1 * s2 * s3 - c3 * s1),
                     },
                     {
                     inverseScale.y * (c3 * s1 * s2 - c1 * s3),
                     inverseScale.y * (c2 * c3),
                     inverseScale.y * (c1 * c3 * s2 + s1 * s3),
                     },
                     {inverseScale.z * (c2 * s1), inverseScale.z * (-s2), inverseScale.z * (c1 * c2)}};
  }

  GameObject GameObject::MakePointLight(const float intensity, const float radius, const glm::vec3 color) {
    GameObject obj{GameObject::CreateGameObject()};
    obj.color = color;
    obj.transform.scale.x = radius;
    obj.pointLight = std::make_unique<PointLightComponent>();
    obj.pointLight->lightIntensity = intensity;
    return obj;
  }

  void PhysicsComponent::ApplyForce(const glm::vec3 force) const noexcept {
    if (isStatic)
      return;
    acceleration += force / mass;
  }
  void PhysicsComponent::ApplyBounce(GameObject &objA, GameObject &objB, const float deltaTime) const noexcept {
    if (objA.physics.isStatic or objB.physics.isStatic) {
      return;
    }

    const glm::vec3 normal{normalize(objA.transform.translation - objB.transform.translation)};
    const glm::vec3 relativeVelocity{velocity - objB.physics.velocity};
    const float velocityAlongNormal{dot(relativeVelocity, normal)};

    if (velocityAlongNormal > 0) {
      return;
    }

    const float impulse{-(1 + bounciness) * velocityAlongNormal / (1 / mass + 1 / objB.physics.mass)};
    const glm::vec3 impulseA{impulse * normal / mass};
    const glm::vec3 impulseB{impulse * normal / objB.physics.mass};
    objA.physics.velocity += impulseA;
    objB.physics.velocity -= impulseB;

    const float overlap{length(objA.transform.translation - objB.transform.translation)};
    const float minDistance{length(objA.model->GetBoundingBoxSize() * objA.transform.scale) / 2.0f +
                            length(objB.model->GetBoundingBoxSize() * objB.transform.scale) / 2.0f};

    if (overlap < minDistance) {
      const float penetrationDepth{minDistance - overlap};
      const glm::vec3 pushAway{normal * penetrationDepth * .5f};

      objA.transform.translation += pushAway * (objB.physics.mass / (mass + objB.physics.mass));
      objB.transform.translation -= pushAway * (mass / (mass + objB.physics.mass));
    }

    objA.transform.translation += objA.physics.velocity * deltaTime;
    objB.transform.translation += objB.physics.velocity * deltaTime;
  }


} // namespace Enforcer
