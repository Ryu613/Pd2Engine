#pragma once

#include "pd/asset/asset.hpp"

namespace pd {
class FileSystem;
class AssetParser {
 public:
  AssetParser() noexcept = default;
  virtual ~AssetParser() = default;
  MOVABLE_ONLY(AssetParser);

  virtual std::unique_ptr<Asset> parse(FileSystem& fs,
                                       const Asset::Info& assetInfo) noexcept = 0;
};
}  // namespace pd