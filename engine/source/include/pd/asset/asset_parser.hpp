#pragma once

#include <expected>

#include "pd/asset/asset.hpp"

namespace pd {
class Asset;
class AssetParser {
 public:
  AssetParser() noexcept = default;
  virtual ~AssetParser() = default;
  MOVABLE_ONLY(AssetParser);

  virtual std::expected<void, AssetError> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd