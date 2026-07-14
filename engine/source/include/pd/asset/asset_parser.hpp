#pragma once

#include "pd/asset/asset.hpp"

namespace pd {
class IAssetParser {
 public:
  IAssetParser() noexcept = default;
  virtual ~IAssetParser() = default;
  DEFAULT_MOVABLE(IAssetParser);
  DELETE_COPY(IAssetParser);

  virtual Result<void> parse(Asset& asset) noexcept = 0;
};
}  // namespace pd