#pragma once

#include "../common/logging/log.hpp"
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

namespace Enforcer {
struct TransformComponent {
  glm::vec3 translation{}; // position offset
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{}; //  cos0 -sin0 sin0 cos0 = circle

  // Y X Z rotation
  glm::mat4 mat4();
  glm::mat3 normalMatrix();
};
class GameObject {
public:
  static GameObject CreateGameObject() {
    static u32 currentId{0};
    LOG_DEBUG(Vulkan, "GameObject{} created", currentId);
    return GameObject{currentId++};
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&) = default;
  // GameObject &operator=(GameObject &&) = default;

  u32 GetId() const noexcept { return id; };

  const u32 id;
  std::shared_ptr<Model> model;
  glm::vec3 color;
  TransformComponent transform;

private:
  explicit GameObject(u32 id) : id{id} {}
};
} // namespace Enforcer
