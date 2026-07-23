#pragma once

#include "pd/scene/entity.hpp"
#include "/pd/resource/resource_alias.hpp"

namespace pd {
struct Name {
  std::string name;
};

struct Transform {
  math::vec3 location{0.0f, 0.0f, 0.0f};
  math::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  math::vec3 scale{1.0f, 1.0f, 1.0f};
  Entity parent{};
  // 左孩子右兄弟
  Entity firstChild{};
  Entity nextSibling{};

  math::mat4 toLocalMatrix() const noexcept {
    auto transMat = glm::translate(glm::mat4{1.0f}, location);
    auto rotateMat   = glm::mat4_cast(rotation);
    auto scaleMat = glm::scale(glm::mat4{1.0f}, scale);
    return transMat * rotateMat * scaleMat;
  }
};

struct Light {
  enum class Type : uint8_t {
    Directional = 1,
  };
  Type LightType = Light::Type::Directional;
  glm::vec3 color;
  float intensity;
};

struct Camera {
  enum class ProjectionType : uint8_t {
    Perspective = 1,
    Orthographic = 2,
  };
  ProjectionType mProjectionType = ProjectionType::Perspective;
  float zNear = 0.0f;
  float zFar = 0.0f;
  float hFov = 0.0f;
  float mAspectRatio = 0.0f;
};
}  // namespace pd