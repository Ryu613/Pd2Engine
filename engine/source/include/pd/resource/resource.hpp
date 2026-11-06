#pragma once

#include "pd/core/utils/traits.hpp"
#include "pd/resource/resource_types.hpp"

namespace pd {
class Backend;
class Resource {
 public:
  virtual ~Resource();
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load(Backend& backend, u32& loadCount) noexcept;
  Result<void> unload(Backend& backend, u32& loadCount) noexcept;

  ResourceIdType variantId() const noexcept { return mVariantId; }
  u64 sourceId() const noexcept { return mSourceId; };
  const std::string& name() const noexcept { return mName; }
  ResourceStatus status() const noexcept { return mStatus; }

 protected:
  explicit Resource(u64 mSourceId, ResourceVariantIdType variantId, std::string name = "Unknown");

  virtual Result<void> doLoad(Backend& backend) noexcept = 0;
  virtual Result<void> doUnload(Backend& backend) noexcept = 0;

 private:
  u64 mSourceId;
  ResourceVariantIdType mVariantId;
  std::string mName;
  ResourceStatus mStatus;
};

template <typename T>
concept DerivedResource = traits::DerivedWithoutBase<T, Resource>;
}  // namespace pd