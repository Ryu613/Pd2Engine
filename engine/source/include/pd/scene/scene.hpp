#pragma once

#include "pd/scene/entity.hpp"
#include "pd/scene/skybox.hpp"
#include "pd/scene/view.hpp"

namespace pd {
class Scene {
 public:
  Scene() noexcept = default;
  ~Scene() = default;
  NO_COPY_MOVE(Scene);

  Entity addEntity(Entity entity) noexcept;
  Entity removeEntity(Entity entity) noexcept;
  [[nodiscard]] bool hasEntity(Entity entity) const noexcept;

  View& createView() noexcept;

 private:
  std::vector<Entity> mEntities;
  std::vector<View*> mViews;
  Skybox mSkyBox;
};
}  // namespace pd