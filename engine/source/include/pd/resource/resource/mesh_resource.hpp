#pragma once

#include "pd/resource/resource.hpp"
#include "pd/resource/resource_types.hpp"
#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_buffer.hpp"

namespace pd {
// struct MeshPrimitive {
//   std::vector<Vertex> vertices;
//   std::vector<uint32_t> indices;
//   //   HwHandle<Material_t> material;
// };
// struct MeshOutput {
//   std::vector<Vertex> vertices;
//   std::vector<uint32_t> indices;
//   struct PrimitiveInfo {
//     uint32_t indexOffset = 0;
//     uint32_t indexCount = 0;
//     // Material material;  // 材质信息
//   };
//   std::vector<PrimitiveInfo> primitives;
// };
struct MeshResource_t;
class MeshResource : public Resource {
 public:
  explicit MeshResource(IdType id) noexcept
      : Resource(id) {}
  ~MeshResource() = default;
  DELETE_COPY(MeshResource);
  DEFAULT_MOVABLE(MeshResource);

  void setRawData(MeshData* pMeshData) noexcept { mRawData = pMeshData; }

  [[nodiscard]] HwHandle<Buffer_t> getVertexBuffer() const noexcept { return mVertexBuffer; }

  [[nodiscard]] HwHandle<Buffer_t> getIndexBuffer() const noexcept { return mIndexBuffer; }

 protected:
  void doLoad(IBackend& backend) noexcept override;
  void doUnload(IBackend& backend) noexcept override;

 private:
  MeshData* mRawData = nullptr;
  HwHandle<Buffer_t> mVertexBuffer;
  HwHandle<Buffer_t> mIndexBuffer;
  //   MaterialInstance* mMaterialInstance = nullptr;
  u32 mVertexCount = 0;
  u32 mIndexCount = 0;
  //   u64 getVertexDataSize() noexcept;
  //   u64 getIndexDataSize() noexcept;
  //   MeshOutput outputData() noexcept;
};
}  // namespace pd