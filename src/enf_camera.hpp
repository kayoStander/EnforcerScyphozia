#pragma once

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
                                 const float near, const float far);

  void SetPerspectiveProjection(const float fovy, const float aspect,
                                const float near, const float far);

  const glm::mat4 &GetProjection() const noexcept { return projectionMatrix; }

private:
  glm::mat4 projectionMatrix{1.f};
};
} // namespace Enforcer
