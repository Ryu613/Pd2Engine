#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class BufferResource : public Resource {
 public:
  struct Info {
    u32 size = 0;
    BufferUsage usage = BufferUsage::VertexBuffer;
  };
  ~BufferResource() = default;
  DELETE_COPY(BufferResource);
  DEFAULT_MOVABLE(BufferResource);

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  friend class ResourceManager;

  struct UploadInfo {
    const void* pData = nullptr;
    u32 dataSize = 0;
    u32 offset = 0;
  };

  Info mInfo;
  HwBufferHandle mBuffer;

  UploadInfo mUploadInfo;

  explicit BufferResource(AssetIdType assetId = invalidAssetId, ResourceVariantIdType variantId = defaultVariantId,
                          std::string name = "buffer")
      : Resource(assetId, variantId, std::move(name)) {}
};

template <>
struct TagInfo<BufferResource_t> {
  using ResourceType = BufferResource;
};
}  // namespace pd