#pragma once

#include "pd/core/entity.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class LightManager {
 public:
  LightManager() noexcept = default;
  ~LightManager() = default;
  NO_COPY_MOVE(LightManager);

  Light* create(Entity e) noexcept;
  Light* getEntity(Entity e) noexcept;
  bool hasEntity(Entity e) noexcept;
  void destroy(Entity e) noexcept;

 private:
  // SOA container
};
}  // namespace pd