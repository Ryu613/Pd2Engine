#pragma once

#include "pd/core/entity.hpp"

namespace pd {
class Scene {
 public:
  Scene() noexcept = default;
  ~Scene() = default;
  NO_COPY_MOVE(Scene);

  Entity addEntity(Entity entity) noexcept;

 private:
};

// helper functions

/**
 * @brief 用模型在场景中构建实体，并返回模型的根节点对应的实体
 *
 * @param scene 场景实例
 * @param model 模型信息
 * @return Entity 模型根节点
 */
// Entity buildEntity(Scene& scene, Model& model) noexcept;
}  // namespace pd