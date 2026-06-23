#pragma once

#include "pd/core/math/math.hpp"
#include "pd/resource/resource.hpp"

namespace pd {
struct MeshResource_t;
struct Vertex {
  math::vec3 position;
  math::vec3 normal;
  math::vec2 uv;
  math::vec3 color;
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