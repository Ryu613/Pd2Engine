#pragma once

#include "pd/scene/entity.hpp"
#include "pd/scene/manager/components_registry.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class RenderableManager {
 public:
  RenderableManager() noexcept = default;
  ~RenderableManager() = default;
  MOVABLE_ONLY(RenderableManager);

  void create(Entity e, Renderable renderable) noexcept;
  void getEntity(Entity e) noexcept;
  bool hasEntity(Entity e) noexcept;
  void destroy(Entity e) noexcept;

  void setTransform(Entity e, Transform transform) noexcept;
  void setParent(Entity e, Entity parent) noexcept;
  void setChild(Entity e, Entity child) noexcept;

 private:
  using Registry = ComponentsRegistry<Renderable>;
  Registry mRegistry;
};
}  // namespace pd