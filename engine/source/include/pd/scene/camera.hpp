#pragma once

#include "pd/core/math/math.hpp"

namespace pd {
struct Camera {
  math::mat4 projection() const noexcept {
    math::mat4 proj = {1.0f};
    if (projectionType == ProjectionType::Perspective) {
      proj = glm::perspective(hFov, aspectRatio, zNear, zFar);
    } else {
      PD_ASSERT_MSG(false, "not impemented!");
    }
    return proj;
  }

  math::mat4 view() const noexcept {
    auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
  }

  enum class ProjectionType : u8 {
    Perspective = 1,
    Orthographic = 2,
  };
  ProjectionType projectionType = ProjectionType::Perspective;
  f32 zNear = 0.01f;
  f32 zFar = 1000.0f;
  f32 hFov = math::PiOver4;
  f32 aspectRatio = 1.778f;
};
}  // namespace pd