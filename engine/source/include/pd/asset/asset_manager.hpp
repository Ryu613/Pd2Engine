#pragma once

#include "pd/platform/file/file_system.hpp"
#include "pd/asset/asset_parser.hpp"

namespace pd {
/**
 * @brief 负责离线资产文件的读取，解析，转换，产出和管理资产信息，供后续使用
 * @note 不负责运行时资源
 */
class AssetManager {
 public:
  explicit AssetManager(FileSystem& fileSystem, EntityManager& entityManager) noexcept;
  ~AssetManager() = default;

  NO_COPY_MOVE(AssetManager);

  /**
   * @brief 解析并转换资产文件，保存资产元数据信息
   *
   * @note 资产拥有者是此管理器
   * @tparam T
   * @param assetInfo
   * @return Asset::AssetId
   */
  template <Asset::Type T>
  Asset* createAsset(const Asset::Info& assetInfo) noexcept {
    // 检查是否已存在此资产, 目前的实现: 把path视为id
    Asset::IdType assetId{assetInfo.path};
    auto it = mAssets.find(assetId);
    if (it != mAssets.end()) {
      return it->second.get();
    }
    // 1. 根据T类型解析资产，生成资产实例
    auto parsedAsset = mParsers[T]->parse(mFileSystem, assetInfo);
    // 2. 记录到资产记录表
    parsedAsset->setId(assetId);  // 只有assetManager有权限设置id, parser不能设置asset id
    mAssets.emplace(assetId, std::move(parsedAsset));

    return mAssets[assetId].get();
  }

 private:
  FileSystem& mFileSystem;
  EntityManager& mEntityManger;
  std::unordered_map<Asset::Type, std::unique_ptr<AssetParser>> mParsers;
  std::unordered_map<Asset::IdType, std::unique_ptr<Asset>> mAssets;

  void initParsers() noexcept;
};
}  // namespace pd