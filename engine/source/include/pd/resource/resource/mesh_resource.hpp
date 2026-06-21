#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
struct MeshResource_t;
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 color;
};
struct Aabb {};
struct MeshPrimitive {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};
class MeshResource : public Resource {
 public:
  MeshResource() noexcept;
  ~MeshResource();
  MOVABLE_ONLY(MeshResource);

  void addPrimitive(const MeshPrimitive& primitive) noexcept {
    mPrimitives.push_back(primitive);
  }

 protected:
  void doLoad() noexcept override;
  void doUnload() noexcept override;

 private:
  std::vector<MeshPrimitive> mPrimitives;

  void init() noexcept;
};
}  // namespace pd