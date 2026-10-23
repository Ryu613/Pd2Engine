#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class BufferResource : public Resource {
 public:
  struct Info {
    u32 size = 0;
    BufferUsage usage = BufferUsage::Unknown;
  };
  ~BufferResource();

  Info info() const noexcept { return mInfo; }
  HwBufferHandle hwHandle() const noexcept { return mBuffer; }

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  Info mInfo;
  HwBufferHandle mBuffer;

  explicit BufferResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Info info);
};
struct BufferResource_t;
}  // namespace pd