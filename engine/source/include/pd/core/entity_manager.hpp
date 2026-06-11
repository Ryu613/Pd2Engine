#pragma once

#include "entt/entt.hpp"

#include "pd/core/entity.hpp"

namespace pd {
/**
 * @brief 实体管理器，负责实体的生命周期和相关功能, 分发entity(即id), 主要用于场景图
 *
 */
class EntityManager {
 public:
  EntityManager() noexcept = default;
  ~EntityManager() = default;
  NO_COPY_MOVE(EntityManager);

  /**
   * @brief Create a Entity object
   *
   * @return Entity
   */
  Entity createEntity() noexcept {
    Entity entity{mRegistry.create()};
    return entity;
  }
  /**
   * @brief Create list of Entities
   *
   * @param n number of entity
   * @return std::vector<Entity>
   */
  std::vector<Entity> createEntity(size_t n) noexcept {
    std::vector<Entity> entities;
    entities.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      entities.push_back(createEntity());
    }
    return entities;
  }

  template <typename... T>
  [[nodiscard]] bool hasComponent(Entity entity) const noexcept {
    return mRegistry.all_of<T...>(entity.mId);
  }

  void destroyEntity(Entity entity) noexcept { mRegistry.destroy(entity.mId); }

  template <typename T, typename... Args>
  void addComponent(Entity entity, Args&&... args) noexcept {
    PD_ASSERT_MSG(!hasComponent<T>(entity), "cannot add duplicated component!");
    mRegistry.emplace<T>(entity.mId, std::forward<Args>(args)...);
  }

  template <typename T>
  void removeComponent(Entity entity) noexcept {
    PD_ASSERT_MSG(!hasComponent<T>(entity), "cannot remove component!");
    mRegistry.remove<T>(entity.mId);
  }

  template <typename T>
  [[nodiscard]] bool getComponent(Entity entity) const noexcept {
    PD_ASSERT_MSG(!hasComponent<T>(entity), "cannot get component!");
    return mRegistry.get<T>(entity.mId);
  }

 private:
  entt::registry mRegistry;
};
}  // namespace pd