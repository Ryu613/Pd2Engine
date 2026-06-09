#pragma once

#include "entt/entity/entity.hpp"

namespace pd {
/**
 * @brief 实体，本质只是id，需要通过manager获取对应component, 目前使用entt实现
 *
 */
class Entity {
 public:
  using IdType = entt::entity;
  // static constexpr uint32_t nullId = UINT32_MAX;

  Entity() noexcept = default;

  [[nodiscard]] bool isNull() const noexcept { return mId != entt::null; }

  [[nodiscard]] IdType GetId() const noexcept { return mId; }

  bool operator==(const Entity& e) const noexcept { return mId == e.mId; }

 private:
  friend class EntityManager;
  explicit Entity(IdType id) noexcept
      : mId(id) {}

  IdType mId;
  // TODO(author): entt has generation?
};
}  // namespace pd
