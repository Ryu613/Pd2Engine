#pragma once

#include "entt/entity/entity.hpp"

namespace pd {
class Entity {
 public:
  using IdType = entt::entity;
  inline static constexpr IdType invalidId = entt::null;

  Entity()
      : mId(invalidId) {}

  ~Entity() = default;
  Entity(const Entity& rhs) = default;
  Entity& operator=(const Entity& rhs) = default;
  Entity(Entity&& rhs) noexcept = default;
  Entity& operator=(Entity&& rhs) noexcept = default;

  // implicit cast to entt::entity
  explicit operator IdType() const { return mId; }

  bool isNull() const noexcept { return mId != invalidId; }

  bool operator==(const Entity& e) const noexcept { return mId == e.mId; }

 private:
  friend class SceneManager;
  IdType mId;

  explicit Entity(IdType id)
      : mId(id) {}
};
}  // namespace pd