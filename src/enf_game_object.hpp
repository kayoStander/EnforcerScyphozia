#pragma once

// #include "../common/logging/log.hpp"
#include "../common/types.hpp"
#include "enf_model.hpp"
#include <array>

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <memory>

namespace Enforcer {
struct TransformComponent {
  glm::vec3 translation{}; // position offset
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{}; //  cos0 -sin0 sin0 cos0 = circle

  // Y X Z rotation
  glm::mat4 mat4();
  glm::mat3 normalMatrix();
};

struct PointLightComponent {
  float lightIntensity{1.f};
};

struct PhysicsComponent {
  glm::vec3 velocity{0.f};
  mutable glm::vec3 acceleration{0.f};
  float mass{10.f};
  bool isStatic{false};

  void ApplyForce(const glm::vec3 force) const noexcept {
    if (isStatic)
      return;
    acceleration += force / mass;
  }

  void Update(float deltaTime, TransformComponent &transform) {
    if (isStatic)
      return;
    velocity += acceleration * deltaTime;
    transform.translation += velocity * deltaTime;
    acceleration = glm::vec3(.0f);
  }
};

class GameObject {
public:
  static GameObject MakePointLight(float intensity = 10.f, float radius = .1f,
                                   glm::vec3 color = glm::vec3{1.f});

  static GameObject CreateGameObject() {
    static u32 currentId{0};
    // LOG_DEBUG(Vulkan, "GameObject{} created", currentId);
    return GameObject{currentId++};
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&) = default;

  u32 GetId() const noexcept { return id; };

  glm::vec3 color;
  TransformComponent transform;
  PhysicsComponent physics;

  std::shared_ptr<Model> model{nullptr};
  std::array<std::shared_ptr<Model>, 4> imposters;
  std::unique_ptr<PointLightComponent> pointLight{nullptr};

  float reflection{.01f};
  u32 imageBind{0};

private:
  explicit GameObject(u32 id) : id{id} {}
  const u32 id;
};
} // namespace Enforcer
