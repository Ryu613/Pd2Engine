#pragma once

namespace pd {
class Node {};
class Model {
 public:
  virtual ~Model() = default;
  MOVABLE_ONLY(Model);

  /**
   * @brief 加载模型，并把资源注册到资源管理器
   *
   * @param resourceMgr
   */
  virtual void load(ResourceManager& resourceMgr) noexcept = 0;
};

class GltfModel {
 public:
  GltfModel(std::string name, std::string path) noexcept
      : mName(std::move(name)),
        mPath(std::move(path)) {}

  void load(ResourceManager& resourceMgr) noexcept;

 private:
  std::string mName;
  std::string mPath;
  std::vector<TextureHandle> mTextures;
  std::vector<MeshHandle> mMeshes;
  std::vector<Node*> mSceneNodes;
};

struct Aabb {};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 color;
};

class Mesh {
 public:
  struct Primitive {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
  };

 private:
  std::vector<Primitive> mPrimitives;
  Aabb boundingBox;
};
}  // namespace pd