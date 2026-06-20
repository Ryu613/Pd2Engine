#pragma once

#include "pd/scene/entity.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class TransformManager {
 public:
  TransformManager() noexcept = default;
  ~TransformManager() = default;
  MOVABLE_ONLY(TransformManager);

  Transform* create(Entity e) noexcept;
  Transform* getEntity(Entity e) noexcept;
  bool hasEntity(Entity e) noexcept;
  void destroy(Entity e) noexcept;

 private:
  // SOA container
};
}  // namespace pd