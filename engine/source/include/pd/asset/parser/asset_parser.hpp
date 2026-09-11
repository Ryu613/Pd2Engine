#pragma once

namespace pd {
class Asset;
class IAssetParser {
 public:
  IAssetParser() = default;
  virtual ~IAssetParser() = default;
  DELETE_COPY(IAssetParser);
  DEFAULT_MOVABLE(IAssetParser);

  virtual Result<void> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd