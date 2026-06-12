#pragma once

#include <expected>

#include "pd/asset/asset.hpp"

namespace pd {
class Asset;
class AssetParser {
 public:
  AssetParser() noexcept = default;
  virtual ~AssetParser() = default;
  NO_COPY_MOVE(AssetParser);

  virtual std::expected<void, AssetError> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd