#pragma once

#include "pd/core/utils/traits.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/resource/resource_types.hpp"

namespace pd {
class Backend;
class Resource {
 public:
  virtual ~Resource();
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load(Backend& backend) noexcept;
  Result<void> unload(Backend& backend) noexcept;

  ResourceIdType variantId() const noexcept { return mVariantId; }
  AssetIdType assetId() const noexcept { return mAssetId; };
  const std::string& name() const noexcept { return mName; }
  ResourceStatus status() const noexcept { return mStatus; }

 protected:
  explicit Resource(AssetIdType assetId, ResourceVariantIdType variantId, std::string name = "Unknown");

  virtual Result<void> doLoad(Backend& backend) noexcept = 0;
  virtual Result<void> doUnload(Backend& backend) noexcept = 0;

 private:
  AssetIdType mAssetId;
  ResourceVariantIdType mVariantId;
  std::string mName;
  ResourceStatus mStatus;
};

template <typename T>
concept DerivedResource = traits::DerivedWithoutBase<T, Resource>;
}  // namespace pd