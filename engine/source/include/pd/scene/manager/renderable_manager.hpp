#pragma once

#include "pd/core/entity.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class RenderableManager {
 public:
  RenderableManager() noexcept = default;
  ~RenderableManager() = default;
  NO_COPY_MOVE(RenderableManager);

  Renderable* create(Entity e) noexcept;
  Renderable* getEntity(Entity e) noexcept;
  bool hasEntity(Entity e) noexcept;
  void destroy(Entity e) noexcept;

 private:
  // SOA container
};
}  // namespace pd