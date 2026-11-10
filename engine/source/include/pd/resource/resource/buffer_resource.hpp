#pragma once

#include "pd/backend/backend_types.hpp"
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

  HwBufferHandle buffer() noexcept { return mBuffer; }

 protected:
  Result<void> doLoad(LoadContext& ctx) noexcept override;
  Result<void> doUnload(LoadContext& ctx) noexcept override;

 private:
  friend class ResourceManager;

  Info mInfo;

  HwBufferHandle mBuffer;

  explicit BufferResource(u64 sourceId, Info bufferInfo, ResourceVariantIdType variantId = defaultVariantId,
                          std::string name = "buffer")
      : Resource(sourceId, variantId, std::move(name)),
        mInfo(std::move(bufferInfo)) {}
};
}  // namespace pd