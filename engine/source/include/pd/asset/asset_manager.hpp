#pragma once

#include <expected>

#include "pd/asset/asset_parser.hpp"

namespace pd {
class FileSystem;
class EntityManager;
class ResourceManager;
/**
 * @brief 负责离线资产文件的读取，解析，转换，产出和管理资产信息，供后续使用
 * @note 不负责运行时资源
 */
class AssetManager {
 public:
  explicit AssetManager(FileSystem& fs, EntityManager& em, ResourceManager& rm) noexcept;
  ~AssetManager() = default;
  NO_COPY_MOVE(AssetManager);

  /**
   * @brief 解析并转换资产文件，保存资产元数据信息
   *
   * @param assetInfo
   * @return std::expected<std::shared_ptr<Asset>, AssetError>
   */
  std::expected<std::shared_ptr<Asset>, AssetError> createAsset(
      const Asset::Info& assetInfo) noexcept;

 private:
  FileSystem& mFileSystem;
  EntityManager& mEntityManger;
  ResourceManager& mResourceManager;
  std::vector<std::unique_ptr<AssetParser>> mParsers;
  std::unordered_map<Asset::IdType, std::shared_ptr<Asset>> mAssets;

  void initParsers() noexcept;
};
}  // namespace pd