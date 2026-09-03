#include "pd/scene/manager/transform_manager.hpp"

namespace pd {
void TransformManager::create(Entity e, Transform transform) noexcept {
  mRegistry.emplace<Transform>(e, transform);
}
}  // namespace pd