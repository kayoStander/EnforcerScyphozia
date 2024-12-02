#include "enf_camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#include "enf_window.hpp"

namespace Enforcer {
  void Camera::SetOrthographicProjection(const float left, const float right, const float top, const float bottom,
                                         const float near, const float far) const {
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
  }

  void Camera::SetPerspectiveProjection(const float fovy, const float aspect, const float near, const float far) const {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > .0f);
    const float tanHalfFovy = std::tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
  }
  void Camera::SetViewDirection(const glm::vec3 position, const glm::vec3 direction, const glm::vec3 up) const {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
  }

  void Camera::SetViewTarget(const glm::vec3 position, const glm::vec3 target, const glm::vec3 up) const {
    SetViewDirection(position, target - position, up);
  }

  void Camera::FollowMouse(const Window& enforcerWindow) const {
    const float height{static_cast<float>(enforcerWindow.GetHeight())};
    const float width{static_cast<float>(enforcerWindow.GetWidth())};

    if (GLFWwindow* window{enforcerWindow.GetGLFWWindow()};
      glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
      glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
      double mouseX{},mouseY{};
      glfwGetCursorPos(window,&mouseX,&mouseY);
      const float rotateX{SENSITIVITY * static_cast<float>(mouseY - height / 2.f) / height};
      const float rotateY{SENSITIVITY * static_cast<float>(mouseX - width / 2.f) / width};

      const glm::mat4 rotationMatrixX{rotate(glm::mat4(1.f),glm::radians(-rotateX),
        normalize(glm::vec3{1.f,0.f,0.f}))};
      const glm::mat4 rotationMatrixY{rotate(glm::mat4(1.f),glm::radians(-rotateY),
        normalize(glm::vec3{0.f,1.f,0.f}))};
      viewMatrix = rotationMatrixY * rotationMatrixX * viewMatrix;

      glfwSetCursorPos(window,width / 2.f,height / 2.f);
    } else {
      glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }
  }

  void Camera::SetViewYXZ(const glm::vec3 position, const glm::vec3 rotation) const {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
  }

  std::array<glm::vec4, 6> Camera::GetFrustumPlanes() const {
    glm::mat4 vpMatrix{projectionMatrix * viewMatrix};

    std::array<glm::vec4, 6> planes;

    // Left plane
    planes[0] = glm::vec4(vpMatrix[0][3] + vpMatrix[0][0], vpMatrix[1][3] + vpMatrix[1][0],
                          vpMatrix[2][3] + vpMatrix[2][0], vpMatrix[3][3] + vpMatrix[3][0]);
    // Right plane
    planes[1] = glm::vec4(vpMatrix[0][3] - vpMatrix[0][0], vpMatrix[1][3] - vpMatrix[1][0],
                          vpMatrix[2][3] - vpMatrix[2][0], vpMatrix[3][3] - vpMatrix[3][0]);
    // Bottom plane
    planes[2] = glm::vec4(vpMatrix[0][3] + vpMatrix[0][1], vpMatrix[1][3] + vpMatrix[1][1],
                          vpMatrix[2][3] + vpMatrix[2][1], vpMatrix[3][3] + vpMatrix[3][1]);
    // Top plane
    planes[3] = glm::vec4(vpMatrix[0][3] - vpMatrix[0][1], vpMatrix[1][3] - vpMatrix[1][1],
                          vpMatrix[2][3] - vpMatrix[2][1], vpMatrix[3][3] - vpMatrix[3][1]);
    // Near plane
    planes[4] = glm::vec4(vpMatrix[0][2], vpMatrix[1][2], vpMatrix[2][2], vpMatrix[3][2]);
    // Far plane
    planes[5] = glm::vec4(vpMatrix[0][3] - vpMatrix[0][2], vpMatrix[1][3] - vpMatrix[1][2],
                          vpMatrix[2][3] - vpMatrix[2][2], vpMatrix[3][3] - vpMatrix[3][2]);

    for (auto &plane: planes) {
      plane = glm::normalize(plane);
    }

    return planes;
  };

} // namespace Enforcer
