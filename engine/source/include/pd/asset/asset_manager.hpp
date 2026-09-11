#pragma once

#include "pd/platform/fs/file_system.hpp"
#include "pd/asset/asset.hpp"
#include "pd/asset/parser/asset_parser.hpp"

namespace pd {
class AssetManager {
 public:
  explicit AssetManager(IFileSystem* fs);
  ~AssetManager();
  DELETE_COPY_MOVE(AssetManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

 private:
  IFileSystem* mFs = nullptr;

  std::vector<std::unique_ptr<IAssetParser>> mParsers;
  std::unordered_map<AssetIdType, std::unique_ptr<Asset>> mAssets;

  u32 mNextId = 0;
};
}  // namespace pd