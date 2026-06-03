#pragma once

#include <expected>

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
   * @param assetInfo
   * @return std::expected<std::shared_ptr<Asset>, AssetError>
   */
  std::expected<std::shared_ptr<Asset>, AssetError> createAsset(
      const Asset::Info& assetInfo) noexcept {
    // 检查是否已存在此资产, 目前的实现: 把path视为id
    const Asset::IdType& assetId = assetInfo.path;
    auto it = mAssets.find(assetId);
    if (it != mAssets.end()) {
      return it->second;
    }
    // 1 创建资产实例
    auto newAsset = std::shared_ptr<Asset>(new Asset(assetId, assetInfo));
    // 2. 生成资产实例
    auto parseResult =
        mParsers[static_cast<size_t>(assetInfo.parseType)]->parse(mFileSystem, *newAsset);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }
    // 3. 记录到资产记录表
    auto [insertedIt, success] = mAssets.emplace(assetId, std::move(newAsset));
    PD_ASSERT_MSG(success, "unexpected asset creations!");

    return insertedIt->second;
  }

 private:
  FileSystem& mFileSystem;
  EntityManager& mEntityManger;
  std::vector<std::unique_ptr<AssetParser>> mParsers;
  std::unordered_map<Asset::IdType, std::shared_ptr<Asset>> mAssets;

  void initParsers() noexcept;
};
}  // namespace pd