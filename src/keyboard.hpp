#pragma once

#include "enf_game_object.hpp"

namespace Enforcer {
#define MOVE_SPEED_WALK 3.f
#define MOVE_SPEED_SPRINT 6.f
  class Keyboard {
  public:
    void MoveInPlaneXZ(const Window& enforcerWindow, float deltaTime, GameObject &gameObject);

    float moveSpeed{1.f};
    const float lookSpeed{1.5f};

  private:
    float speed{3.f};

    enum KeyMappings {
      moveLeft = GLFW_KEY_A,
      moveRight = GLFW_KEY_D,
      moveForward = GLFW_KEY_W,
      moveBackward = GLFW_KEY_S,
      moveUp = GLFW_KEY_E,
      moveDown = GLFW_KEY_Q,
      lookLeft = GLFW_KEY_LEFT,
      lookRight = GLFW_KEY_RIGHT,
      lookUp = GLFW_KEY_UP,
      lookDown = GLFW_KEY_DOWN,
      Sprint = GLFW_KEY_LEFT_SHIFT
    };
  }; // temp
} // namespace Enforcer
