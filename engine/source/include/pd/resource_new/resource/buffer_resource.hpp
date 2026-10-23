#pragma once

#include "pd/resource_new/resource.hpp"

namespace pd {
class BufferResource : public Resource {
 public:
  BufferResource(ResourceIdType id, AssetIdType assetId, std::string name)
      : Resource(id, assetId, std::move(name)) {}

  ~BufferResource() = default;
  DELETE_COPY(BufferResource);
  DEFAULT_MOVABLE(BufferResource);

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  HwBufferHandle mBuffer;
  u32 size;
};

template <>
struct TagInfo<BufferResource_t> {
  using ResourceType = BufferResource;
};
}  // namespace pd