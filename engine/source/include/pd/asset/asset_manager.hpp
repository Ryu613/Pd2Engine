#pragma once

#include "pd/platform/fs/file_system.hpp"
#include "pd/asset/asset.hpp"
#include "pd/asset/parser/asset_parser.hpp"
#include "pd/core/utils/map.hpp"

namespace pd {
class AssetManager {
 public:
  explicit AssetManager(IFileSystem* fs);
  ~AssetManager();
  DELETE_COPY_MOVE(AssetManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  Result<AssetHandle> createAsset(const Asset::CreateInfo& assetInfo) noexcept;

 private:
  IFileSystem* mFs = nullptr;

  std::vector<std::unique_ptr<IAssetParser>> mParsers;
  // std::vector<std::unique_ptr<ITextureCompiler>> mTexCompilers;
  
  using AssetRegistry = util::RobinMap<AssetPathType, AssetIdType, util::StringHasher>;
  AssetRegistry mRegistry;

  using AssetStorage = util::RobinMap<AssetIdType, std::unique_ptr<Asset>>;
  AssetStorage mStorage;

  u32 mNextId = 0;

  void initParsers() noexcept;

  u32 nextId() noexcept;
};
}  // namespace pd