#pragma once

namespace pd {
class Asset;
class AssetManager;
class IAssetParser {
 public:
  IAssetParser() = default;
  virtual ~IAssetParser() = default;
  DELETE_COPY_MOVE(IAssetParser);

  virtual Result<void> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd