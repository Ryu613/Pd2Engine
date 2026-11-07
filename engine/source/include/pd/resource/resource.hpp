#pragma once

#include "pd/core/utils/traits.hpp"
#include "pd/resource/resource_types.hpp"

namespace pd {
class Backend;
class ResourceManager;
struct BufferAllocInfo {
  const void* pData = nullptr;
  u32 dataSize = 0;
  u32 offset = 0;
};
class Resource {
 public:
  struct LoadContext {
    ResourceManager& mgr;
    Backend& backend;
    u32& loadCount;
  };
  virtual ~Resource();
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load(LoadContext& ctx) noexcept;
  Result<void> unload(LoadContext& ctx) noexcept;

  ResourceIdType variantId() const noexcept { return mVariantId; }
  u64 sourceId() const noexcept { return mSourceId; };
  const std::string& name() const noexcept { return mName; }
  ResourceStatus status() const noexcept { return mStatus; }

 protected:
  explicit Resource(u64 mSourceId, ResourceVariantIdType variantId, std::string name = "Unknown");

  virtual Result<void> doLoad(LoadContext& ctx) noexcept = 0;
  virtual Result<void> doUnload(LoadContext& ctx) noexcept = 0;

 private:
  u64 mSourceId;
  ResourceVariantIdType mVariantId;
  std::string mName;
  ResourceStatus mStatus;
};

template <typename T>
concept DerivedResource = traits::DerivedWithoutBase<T, Resource>;
}  // namespace pd