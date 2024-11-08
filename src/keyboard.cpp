#include "keyboard.hpp"

#include <cmath>
#include <limits>

namespace Enforcer {
void Keyboard::MoveInPlaneXZ(GLFWwindow *window, const float deltaTime,
                             GameObject &gameObject) {
  glm::vec3 rotate{0.f};
  if (glfwGetKey(window, KeyMappings::lookRight) == GLFW_PRESS) {
    rotate.y += 1.f;
  }
  if (glfwGetKey(window, KeyMappings::lookLeft) == GLFW_PRESS) {
    rotate.y -= 1.f;
  }

  if (glfwGetKey(window, KeyMappings::lookUp) == GLFW_PRESS) {
    rotate.x += 1.f;
  }
  if (glfwGetKey(window, KeyMappings::lookDown) == GLFW_PRESS) {
    rotate.x -= 1.f;
  }

  if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
    gameObject.transform.rotation +=
        lookSpeed * deltaTime * glm::normalize(rotate);
  }

  gameObject.transform.rotation.x =
      glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f); // no upsideDown
  gameObject.transform.rotation.y =
      glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

  const float yaw = gameObject.transform.rotation.y;
  const glm::vec3 forwardDirection{std::sin(yaw), 0.f, std::cos(yaw)};
  const glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
  constexpr glm::vec3 upDirection{0.f, -1.f, 0.f};

  glm::vec3 moveDirection{0.f};
  if (glfwGetKey(window, KeyMappings::moveForward) == GLFW_PRESS) {
    moveDirection += forwardDirection;
  }
  if (glfwGetKey(window, KeyMappings::moveBackward) == GLFW_PRESS) {
    moveDirection -= forwardDirection;
  }
  if (glfwGetKey(window, KeyMappings::moveRight) == GLFW_PRESS) {
    moveDirection += rightDirection;
  }
  if (glfwGetKey(window, KeyMappings::moveLeft) == GLFW_PRESS) {
    moveDirection -= rightDirection;
  }
  if (glfwGetKey(window, KeyMappings::moveUp) == GLFW_PRESS) {
    moveDirection += upDirection;
  }
  if (glfwGetKey(window, KeyMappings::moveDown) == GLFW_PRESS) {
    moveDirection -= upDirection;
  }

  if (glfwGetKey(window, KeyMappings::Sprint) == GLFW_PRESS) {
    moveSpeed = MOVE_SPEED_SPRINT;
  } else {
    moveSpeed = MOVE_SPEED_WALK;
  }

  if (glm::dot(moveDirection, moveDirection) >
      std::numeric_limits<float>::epsilon()) {
    gameObject.transform.translation +=
        moveSpeed * deltaTime * glm::normalize(moveDirection);
  }
}
} // namespace Enforcer
