#pragma once

#include "pd/resource/resource.hpp"
#include "pd/resource/resource_ref.hpp"
#include "pd/resource/resource/buffer_resource.hpp"

namespace pd {
class MeshResource : public Resource {
 public:
  struct Info {
    u32 vertexCount = 0;
    u32 indexCount = 0;
  };
  struct SubMesh {
    u32 firstIndex = 0;
    u32 indexCount = 0;
    i32 vertexOffset = 0;
    u32 materialSlot = 0;

    // 由asset mgr保证load时存在
    std::span<const VertexData> vertexData;
    std::span<const u32> indexData;
  };

  ~MeshResource() = default;
  DELETE_COPY(MeshResource);
  DEFAULT_MOVABLE(MeshResource);

  const ResourceRef<BufferResource_t>& vertexBuffer() const noexcept { return mVertexBuffer; }
  const ResourceRef<BufferResource_t>& indexBuffer() const noexcept { return mIndexBuffer; }

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  friend class ResourceManager;

  Info mInfo;

  ResourceRef<BufferResource_t> mVertexBuffer;

  ResourceRef<BufferResource_t> mIndexBuffer;

  std::vector<SubMesh> mSubMeshes;

  explicit MeshResource(AssetIdType assetId, ResourceVariantIdType variantId = defaultVariantId,
                        std::string name = "mesh")
      : Resource(assetId, variantId, std::move(name)) {}
};

template <>
struct TagInfo<MeshResource_t> {
  using ResourceType = MeshResource;
};
}  // namespace pd