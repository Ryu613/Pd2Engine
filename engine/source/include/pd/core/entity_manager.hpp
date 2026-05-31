#pragma once

#include "entt/entt.hpp"

#include "pd/core/entity.hpp"

namespace pd {
/**
 * @brief 实体管理器，负责实体的生命周期和相关功能
 *
 */
class EntityManager {
 public:
  Entity createEntity() noexcept;

  void destroyEntity(Entity entity) noexcept;

  template <typename T, typename... Args>
  void addComponent(Args&&... args) noexcept;

  template <typename T>
  void removeComponent() noexcept;

  template <typename T>
  bool getComponent() noexcept;

 private:
  entt::registry mRegistry;
};
}  // namespace pd