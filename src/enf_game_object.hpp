#pragma once

#include "../common/types.hpp"
#include "enf_model.hpp"

#include <memory>

struct Transform2DComponent {
  glm::vec2 translation{}; // position offset
  glm::vec2 scale{1.f, 1.f};
  float rotation; //  cos0 -sin0 sin0 cos0 = circle

  glm::mat2 mat2() noexcept {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    const glm::mat2 rotationMat{{c, s}, {-s, c}};

    const glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
    return rotationMat * scaleMat;
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
  Transform2DComponent transform2D;

private:
  explicit GameObject(u32 id) : id{id} {}
};
} // namespace Enforcer
