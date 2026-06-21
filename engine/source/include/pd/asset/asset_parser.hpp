#pragma once

#include "pd/asset/asset.hpp"

namespace pd {
class IAssetParser {
 public:
  IAssetParser() noexcept = default;
  virtual ~IAssetParser() = default;
  MOVABLE_ONLY(IAssetParser);

  virtual Asset::AssetResult<void> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd