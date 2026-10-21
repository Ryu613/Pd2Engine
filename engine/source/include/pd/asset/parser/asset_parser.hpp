#pragma once

#include "pd/asset/asset_types.hpp"

namespace pd {
class Asset;
class AssetManager;
class IAssetParser {
 public:
  IAssetParser() = default;
  virtual ~IAssetParser() = default;
  DELETE_COPY(IAssetParser);
  DEFAULT_MOVABLE(IAssetParser);

  virtual Result<void> parse(AssetManager& mgr, Asset& asset) noexcept = 0;
};
}  // namespace pd