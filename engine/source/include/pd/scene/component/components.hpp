#pragma once

#include "pd/core/entity.hpp"
#include "pd/resource/resource_handle.hpp"
#include "pd/rendering/resource/mesh_resource.hpp"
#include "pd/rendering/resource/material_resource.hpp"

namespace pd {
struct Name {
  std::string name;
};

struct Transform {
  glm::vec3 location{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  Entity parent;
  std::vector<Entity> children;
};

struct Light {
  enum class Type : uint8_t {
    Directional = 1,
  };
  Type LightType = Light::Type::Directional;
  glm::vec3 color;
  float intensity;
};

struct Renderable {
  ResourceHandle<MeshResource> meshHandle;
  ResourceHandle<MaterialResource> materialHandle;
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