#pragma once

#include "pd/core/entity.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
class CameraManager {
 public:
  CameraManager() noexcept = default;
  ~CameraManager() = default;
  NO_COPY_MOVE(CameraManager);

  Camera* create(Entity e) noexcept;
  Camera* getInstance(Entity e) noexcept;
  bool hasInstance(Entity e) noexcept;
  void destroy(Entity e) noexcept;

 private:
  // SOA container
};
}  // namespace pd