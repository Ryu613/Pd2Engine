#pragma once

#include "pd/scene/entity.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class CameraManager {
 public:
  CameraManager() noexcept = default;
  ~CameraManager() = default;
  MOVABLE_ONLY(CameraManager);

  Camera* create(Entity e) noexcept;
  Camera* getInstance(Entity e) noexcept;
  bool hasInstance(Entity e) noexcept;
  void destroy(Entity e) noexcept;

 private:
  // SOA container
};
}  // namespace pd