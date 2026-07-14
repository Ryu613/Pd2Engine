#pragma once

#include <span>

#include "pd/scene/entity.hpp"
#include "pd/scene/skybox.hpp"
#include "pd/scene/view.hpp"

namespace pd {
class Scene {
 public:
  Scene() noexcept = default;
  ~Scene() = default;
  MOVABLE_ONLY(Scene);

  void addEntity(Entity entity) noexcept;
  void removeEntity(Entity entity) noexcept;
  [[nodiscard]] bool hasEntity(Entity entity) const noexcept;

  View& createView() noexcept;

 private:
  friend class SceneManager;
  std::vector<Entity> mEntities;
  std::vector<View> mViews;
  Skybox mSkyBox;
};
}  // namespace pd