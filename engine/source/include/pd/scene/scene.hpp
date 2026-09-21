#pragma once

#include "pd/scene/entity.hpp"

namespace pd {
class Scene {
 public:
  const auto& getEntities() const noexcept { return mEntities; }

 private:
  std::vector<Entity> mEntities;
};
}  // namespace pd