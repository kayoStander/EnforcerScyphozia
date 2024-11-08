#pragma once

// #include "../common/types.hpp"

#include <array>
#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#error "No GLM recognized in the system"
#endif

namespace Enforcer {
class Camera {
public:
  void SetOrthographicProjection(const float left, const float right,
                                 const float top, const float bottom,
                                 const float near, const float far)const;
  void SetPerspectiveProjection(const float fovy, const float aspect,
                                const float near, const float far) const;

  void SetViewDirection(const glm::vec3 /*pos*/ position,
                        const glm::vec3 /*rot*/ direction,
                        const glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}) const;
  void SetViewTarget(const glm::vec3 position, const glm::vec3 target,
                     const glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}) const;
  void SetViewYXZ(const glm::vec3 position, const glm::vec3 rotation) const;

  glm::mat4 &GetProjection() const noexcept { return projectionMatrix; }
  glm::mat4 &GetView() const noexcept { return viewMatrix; }
  glm::mat4 &GetInverseView() const noexcept { return inverseViewMatrix; }
  glm::vec3 getPosition() const noexcept {
    return glm::vec3{inverseViewMatrix[3]};
  }
  std::array<glm::vec4, 6> GetFrustumPlanes() const;

private:
  mutable glm::mat4 projectionMatrix{1.f};
  mutable glm::mat4 viewMatrix{1.f};
  mutable glm::mat4 inverseViewMatrix{1.f};
};
} // namespace Enforcer
