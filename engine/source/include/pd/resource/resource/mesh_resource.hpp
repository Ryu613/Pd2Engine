#pragma once

#include "pd/core/math/math.hpp"
#include "pd/resource/resource.hpp"
#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_buffer.hpp"

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
  //   HwHandle<Material_t> material;
};
struct MeshOutput {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  struct PrimitiveInfo {
    uint32_t indexOffset = 0;
    uint32_t indexCount = 0;
    // Material material;  // 材质信息
  };
  std::vector<PrimitiveInfo> primitives;
};
class MeshResource : public Resource {
 public:
  struct Properties {
    IdType id{};
  };
  MeshResource() noexcept = default;
  explicit MeshResource(IdType id) noexcept
      : Resource(id) {}
  explicit MeshResource(Properties props) noexcept;
  ~MeshResource();
  MOVABLE_ONLY(MeshResource);

  void addPrimitive(const MeshPrimitive& primitive) noexcept {
    mPrimitives.push_back(primitive);
  }

  //   void draw(IBackend* backend) noexcept;

  MeshOutput outputData() noexcept;

 protected:
  void doLoad(IBackend& backend) noexcept override;
  void doUnload(IBackend& backend) noexcept override;

 private:
  Properties mProps;
  std::vector<MeshPrimitive> mPrimitives;
  HwHandle<Buffer_t> mVertexBuffer;
  HwHandle<Buffer_t> mIndexBuffer;
  //   MaterialInstance* mMaterialInstance = nullptr;
  u32 mVertexCount = 0;

  u64 getVertexDataSize() noexcept;
  u64 getIndexDataSize() noexcept;
};
}  // namespace pd