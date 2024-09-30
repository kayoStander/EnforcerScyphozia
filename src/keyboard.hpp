#pragma once

#include "enf_game_object.hpp"
#include "enf_window.hpp"

namespace Enforcer {
class Keyboard {
public:
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
    lookDown = GLFW_KEY_DOWN
  };

  void MoveInPlaneXZ(GLFWwindow *window, const float deltaTime,
                     GameObject &gameObject);

  const float moveSpeed{3.f};
  const float lookSpeed{1.5f};

}; // temp
} // namespace Enforcer
