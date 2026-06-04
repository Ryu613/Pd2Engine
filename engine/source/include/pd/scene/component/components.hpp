#pragma once

#include "pd/core/entity.hpp"

namespace pd {
struct Name {
  std::string name;
};

struct Transform {
  glm::vec3 location{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct Parent {
  Entity entity{};
};
}  // namespace pd