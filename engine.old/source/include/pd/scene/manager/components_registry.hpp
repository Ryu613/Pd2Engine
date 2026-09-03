#pragma once

#include "entt/entt.hpp"

#include "pd/scene/entity.hpp"

namespace pd {
template <typename T>
concept TrivialComponent = std::semiregular<T>                 // 可复制、可移动、可默认构造
                           && std::is_trivially_copyable_v<T>  // 可平凡拷贝
                           && std::is_trivially_destructible_v<T>;  // 可默认析构
/**
 * @brief soa容器，存储entity对应的所有component
 *
 * @tparam T...
 */
template <TrivialComponent... Ts>
class ComponentsRegistry {
 public:
  ComponentsRegistry() noexcept = default;
  ~ComponentsRegistry() = default;
  explicit ComponentsRegistry(entt::registry* pRegistry) noexcept;
  MOVABLE_ONLY(ComponentsRegistry);

  template <TrivialComponent T, typename... Args>
  T& emplace(Entity e, Args&&... args) noexcept {
    return mRegistry.emplace<T>(entt::entity(e), std::forward<Args>(args)...);
  }

  [[nodiscard]] bool exist(Entity e) const noexcept;

  Entity get(Entity e) const noexcept;

 private:
  // TODO(ryu613): my own soa container not fully tested, use entt's instead for now
  entt::registry mRegistry;
};
}  // namespace pd