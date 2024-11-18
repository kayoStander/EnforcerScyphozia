#pragma once

// #include "../common/types.hpp"

#include <array>
#include <backends/imgui_impl_glfw.h>

#include "enf_window.hpp"
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
#define SENSITIVITY 60.f
  public:
    void SetOrthographicProjection(float left, float right, float top, float bottom,
                                   float near, float far) const;
    void SetPerspectiveProjection(float fovy, float aspect, float near, float far) const;

    void SetViewDirection(glm::vec3 /*pos*/ position, glm::vec3 /*rot*/ direction,
                          glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}) const;
    void SetViewTarget(glm::vec3 position, glm::vec3 target,
                       glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}) const;
    void SetViewYXZ(glm::vec3 position, glm::vec3 rotation) const;
    void FollowMouse(const Window& enforcerWindow) const;

    [[nodiscard]] glm::mat4 &GetProjection() const noexcept { return projectionMatrix; }
    [[nodiscard]] glm::mat4 &GetView() const noexcept { return viewMatrix; }
    [[nodiscard]] glm::mat4 &GetInverseView() const noexcept { return inverseViewMatrix; }
    [[nodiscard]] glm::vec3 getPosition() const noexcept { return glm::vec3{inverseViewMatrix[3]}; }
    [[nodiscard]] std::array<glm::vec4, 6> GetFrustumPlanes() const;

  private:
    mutable glm::mat4 projectionMatrix{1.f};
    mutable glm::mat4 viewMatrix{1.f};
    mutable glm::mat4 inverseViewMatrix{1.f};
  };
} // namespace Enforcer
