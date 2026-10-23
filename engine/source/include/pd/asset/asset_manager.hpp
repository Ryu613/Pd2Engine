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
 * @todo 复合资产交叉引用去重
 * @todo texture转码压缩
 * @todo 资产并行处理
 * @todo 健壮和容错处理
 */
class AssetManager {
 public:
  explicit AssetManager(IFileSystem& fs);
  ~AssetManager();
  DELETE_COPY_MOVE(AssetManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  Result<AssetHandle> createAsset(Asset::MetaInfo metaInfo) noexcept;

  Result<Asset*> getAsset(AssetHandle handle) noexcept;

  Result<void> clearCache() noexcept;

 private:
  IFileSystem* mFs = nullptr;

  // 资产解析器(处理器)
  std::vector<std::unique_ptr<IAssetParser>> mParsers;

  // 注册表(key: 标准化的绝对路径std::string, value: 资产id(u32))
  using AssetRegistry = util::RobinMap<AssetPathType, AssetIdType, util::StringHasher>;
  AssetRegistry mRegistry;

  // 资产缓存
  using AssetStorage = util::RobinMap<AssetIdType, std::unique_ptr<Asset>>;
  AssetStorage mStorage;

  // todo: 本版暂未实现并行处理
  std::atomic<AssetIdType> mNextId = 1;

  void initParsers() noexcept;

  AssetIdType nextId() noexcept;

  std::string getNormalizedPath(const std::string& str) const noexcept;
};

}  // namespace pd