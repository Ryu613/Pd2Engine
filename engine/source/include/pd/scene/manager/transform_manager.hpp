#pragma once

#include "pd/scene/entity.hpp"
#include "pd/scene/component/components.hpp"
#include "pd/scene/manager/components_registry.hpp"

namespace pd {
class TransformManager {
 public:
  TransformManager() noexcept = default;
  ~TransformManager() = default;
  MOVABLE_ONLY(TransformManager);

  void create(Entity e, Transform transform) noexcept;
  void getEntity(Entity e) noexcept;
  bool hasEntity(Entity e) noexcept;
  void destroy(Entity e) noexcept;

  void setTransform(Entity e, Transform transform) noexcept;
  void setParent(Entity e, Entity parent) noexcept;
  void setChild(Entity e, Entity child) noexcept;

 private:
  using Registry = ComponentsRegistry<Transform>;
  Registry mRegistry;
};
}  // namespace pd