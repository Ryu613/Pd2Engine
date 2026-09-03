#include "pd/scene/scene.hpp"

namespace pd {
void Scene::addEntity(Entity entity) noexcept { mEntities.push_back(entity); }
void Scene::removeEntity(Entity entity) noexcept {}
[[nodiscard]] bool Scene::hasEntity(Entity entity) const noexcept { return false; }

View& Scene::createView() noexcept {
  View view;
  mViews.push_back(std::move(view));
  return mViews.back();
}
}  // namespace pd