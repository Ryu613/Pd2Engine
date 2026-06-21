#pragma once

#include "pd/asset/asset_parser.hpp"

namespace pd {
class IFileSystem;
class ResourceManager;
/**
 * @brief 负责离线资产文件的读取，解析，转换，产出和管理资产信息，供后续使用
 * @note 不负责运行时资源
 * @todo 目前解析完直接注册到resource manager，当前阶段没进一步拆分
 */
class AssetManager {
 public:
  AssetManager() noexcept = default;
  explicit AssetManager(IFileSystem* fs, ResourceManager* rm) noexcept;
  ~AssetManager() = default;
  MOVABLE_ONLY(AssetManager);

  /**
   * @brief 解析并转换资产文件，保存资产元数据信息
   *
   * @param assetInfo
   * @return std::expected<std::shared_ptr<Asset>, AssetError>
   */
  Asset::AssetResult<Asset*> createAsset(const Asset::CreateInfo& assetInfo) noexcept;

 private:
  IFileSystem* mFileSystem = nullptr;
  ResourceManager* mResourceManager = nullptr;
  std::vector<std::unique_ptr<IAssetParser>> mParsers;
  std::unordered_map<Asset::IdType, std::unique_ptr<Asset>> mAssets;

  void initParsers() noexcept;
};
}  // namespace pd