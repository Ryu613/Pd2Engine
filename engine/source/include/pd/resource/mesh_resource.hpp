#pragma once

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class MeshResource : public Resource {
 public:
  struct MeshDesc {
    std::string_view debugName;
    const void* pData = nullptr;
    u64 dataSize = 0;
    u32 dataOffset = 0;
  };
  struct IndexDesc {
    std::string_view debugName;
    const void* pData = nullptr;
    u64 dataSize = 0;
    u32 dataOffset = 0;
    u32 indexCount = 0;
  };

  struct BindingInfo {
    HwBufferHandle vertexBuffer;
    u32 vertexOffset = 0;
    HwBufferHandle indexBuffer;
    u32 indexOffset = 0;
    u32 indexCount = 0;
  };

  ~MeshResource() override;
  DELETE_COPY(MeshResource);
  DEFAULT_MOVABLE(MeshResource);

  BindingInfo getBindingInfo() const noexcept {
    BindingInfo info{
        .vertexBuffer = mVertexBuffer,
        .vertexOffset = mMeshDesc.dataOffset,
        .indexBuffer = mIndexBuffer,
        .indexOffset = mIndexDesc.dataOffset,
        .indexCount = mIndexDesc.indexCount,
    };

    return info;
  }

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  Backend* mBackend = nullptr;

  MeshDesc mMeshDesc;
  IndexDesc mIndexDesc;
  HwBufferHandle mVertexBuffer;
  HwBufferHandle mIndexBuffer;

  explicit MeshResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend);

  void setMeshDesc(const MeshDesc& meshDesc) noexcept { mMeshDesc = meshDesc; }
  void setIndexData(const IndexDesc& indexDesc) noexcept { mIndexDesc = indexDesc; };
};
}  // namespace pd