#pragma once

#include "pd/core/utils/traits.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/resource_new/resource_types.hpp"

namespace pd {
class Backend;
class Resource {
 public:
  virtual ~Resource();
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load() noexcept;
  Result<void> unload() noexcept;

  ResourceIdType id() const noexcept { return mId; }
  AssetIdType assetId() const noexcept { return mAssetId; };
  const std::string& name() const noexcept { return mName; }
  ResourceStatus status() const noexcept { return mStatus; }

 protected:
  explicit Resource(ResourceIdType id, AssetIdType assetId, const std::string& name);

  virtual Result<void> doLoad(Backend& backend) noexcept = 0;
  virtual Result<void> doUnload(Backend& backend) noexcept = 0;

 private:
  ResourceIdType mId;
  AssetIdType mAssetId;
  std::string mName;
  ResourceStatus mStatus;
};

template <typename T>
concept DerivedResource = traits::DerivedWithoutBase<T, Resource>;
}  // namespace pd