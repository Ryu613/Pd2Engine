#pragma once

#include "pd/platform/fs/file_system.hpp"
#include "pd/asset/asset.hpp"
#include "pd/asset/parser/asset_parser.hpp"
#include "pd/core/utils/map.hpp"

namespace pd {
/**
 * @brief 负责资产文件读取，解析，编译，转换为引擎内部格式
 *
 * 拥有解析后的资产数据，缓存在此处，外部通过句柄访问
 * @todo 合并node引用的重复mesh
 * @todo texture压缩
 * @todo mesh optimizer集成
 * @todo 把资源注册交给asset mgr
 */
class AssetManager {
 public:
  explicit AssetManager(IFileSystem* fs);
  ~AssetManager();
  DELETE_COPY_MOVE(AssetManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  Result<AssetHandle> createAsset(const Asset::MetaInfo& meta) noexcept;
  Result<Asset*> getAsset(AssetHandle handle) noexcept;

 private:
  IFileSystem* mFs = nullptr;

  // 资产解析器(处理器)
  std::vector<std::unique_ptr<IAssetParser>> mParsers;

  using AssetRegistry = util::RobinMap<AssetPathType, AssetIdType, util::StringHasher>;
  AssetRegistry mRegistry;

  // 资产缓存，主要避免重复处理
  using AssetStorage = util::RobinMap<AssetIdType, std::unique_ptr<Asset>>;
  AssetStorage mStorage;

  u32 mNextId = 0;

  void initParsers() noexcept;

  u32 nextId() noexcept;
};
}  // namespace pd