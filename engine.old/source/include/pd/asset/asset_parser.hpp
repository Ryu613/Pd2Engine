#pragma once

#include "pd/asset/asset.hpp"

namespace pd {
class IAssetParser {
 public:
  IAssetParser() noexcept = default;
  virtual ~IAssetParser() = default;
  DELETE_COPY(IAssetParser);
  DEFAULT_MOVABLE(IAssetParser);

  virtual Result<void> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd