#pragma once

#include "pd/resource_new/resource.hpp"
#include "pd/resource_new/resource_ref.hpp"
#include "pd/resource_new/resource/buffer_resource.hpp"

namespace pd {
class MeshResource : public Resource {
 public:
  struct SubMesh {
    u32 firstIndex = 0;
    u32 indexCount = 0;
    i32 baseVertex = 0;
    u32 materialSlot = 0;
  };

  MeshResource(ResourceIdType id, AssetIdType assetId, std::string name)
      : Resource(id, assetId, std::move(name)) {}

  ~MeshResource() = default;
  DELETE_COPY(MeshResource);
  DEFAULT_MOVABLE(MeshResource);

  const ResourceRef<BufferResource_t>& vertexBuffer() const noexcept { return mVertexBuffer; }
  const ResourceRef<BufferResource_t>& indexBuffer() const noexcept { return mIndexBuffer; }

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  ResourceRef<BufferResource_t> mVertexBuffer;
  u32 vertexCount = 0;
  ResourceRef<BufferResource_t> mIndexBuffer;
  u32 indexCount = 0;
  std::vector<SubMesh> mSubMeshes;
};

template <>
struct TagInfo<MeshResource_t> {
  using ResourceType = MeshResource;
};
}  // namespace pd