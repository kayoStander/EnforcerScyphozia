#pragma once

#include "../common/types.hpp"
#include "enf_model.hpp"

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#error "No GLM recognized in the system"
#endif

#include <memory>

struct TransformComponent {
  glm::vec3 translation{}; // position offset
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{}; //  cos0 -sin0 sin0 cos0 = circle

  // Y X Z rotation
  glm::mat4 mat4() {
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
};

namespace Enforcer {
class GameObject {
public:
  static GameObject CreateGameObject() {
    static u32 currentId{0};
    return GameObject{currentId++};
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&) = default;
  // GameObject &operator=(GameObject &&) = default;

  const u32 id;
  std::shared_ptr<Model> model;
  glm::vec3 color;
  TransformComponent transform;

private:
  explicit GameObject(u32 id) : id{id} {}
};
} // namespace Enforcer
