#include "pd/scene/scene.hpp"

namespace pd {
void Scene::addEntity(Entity entity) noexcept {}
void Scene::removeEntity(Entity entity) noexcept {}
[[nodiscard]] bool Scene::hasEntity(Entity entity) const noexcept { return false; }

View& createView() noexcept;
}  // namespace pd