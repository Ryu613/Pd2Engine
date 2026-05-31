#pragma once

#include "pd/core/entity.hpp"

namespace pd {
struct Transform {
  glm::vec3 pos{0.0f, 0.0f, 0.0f};
  glm::quat rotation{0.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
};
class SceneNode {
 public:
  SceneNode() noexcept;
  virtual ~SceneNode() = default;
  MOVABLE_ONLY(SceneNode);

  virtual void update();
  void addChild(SceneNode* node);

 private:
  std::string mName;
  Entity mEntity;
  std::vector<SceneNode*> mChildren;
};

class GeometryNode : public SceneNode {
 public:
  void update() override {
    // draw
    SceneNode::update();
  }

 private:
};

class SceneGraph {
 public:
  SceneGraph() noexcept = default;
  ~SceneGraph() = default;

  MOVABLE_ONLY(SceneGraph);

  void addEntity(SceneNode& node) noexcept;

  SceneNode& getRoot() noexcept { return mRoot; }

 private:
  Entity mRoot;
  std::vector<Entity> mEntities;
};

}  // namespace pd