#include "pd/asset/asset_manager.hpp"

#include "pd/platform/file/file_system.hpp"
#include "pd/asset/parser/gltf_parser.hpp"

namespace pd {

void AssetManager::initialize(IFileSystem* fs, ResourceManager* rm) noexcept {
  if (mInitialized) {
    return;
  }
  mFileSystem = fs;
  mResourceManager = rm;

  initParsers();
  mInitialized = true;
}

AssetManager::~AssetManager() { destroy(); }

void AssetManager::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
  mInitialized = false;
}

Asset::AssetResult<Asset*> AssetManager::createAsset(
    const Asset::CreateInfo& assetInfo) noexcept {
  // 检查文件是否存在，类型是否正确, 文件是否可读
  const auto& assetPath = assetInfo.path;
  auto& fs = *mFileSystem;
  if (!fs.exists(assetPath) || !fs.isFile(assetPath)) {
    log::error("asset path is illegal: {}", assetPath);
    return std::unexpected(Asset::Error::FileNotFound);
  }
  // 检查是否已存在此资产, 目前的实现: 把path视为id
  const Asset::IdType& assetId = assetInfo.path;
  auto it = mAssets.find(assetId);
  if (it != mAssets.end()) {
    return it->second.get();
  }
  // 1 创建资产实例
  auto newAsset = std::unique_ptr<Asset>(new Asset(assetId, assetInfo));
  // 2. 生成资产实例
  auto parseResult = mParsers[static_cast<size_t>(assetInfo.parseType)]->parse(*newAsset);
  if (!parseResult) {
    return std::unexpected(parseResult.error());
  }
  // 3. 记录到资产记录表
  auto [insertedIt, success] = mAssets.emplace(assetId, std::move(newAsset));
  PD_ASSERT_MSG(success, "unexpected asset creations!");

  return insertedIt->second.get();
}

void AssetManager::initParsers() noexcept {
  mParsers.reserve(8);
  auto gltfParser = std::make_unique<GltfParser>(mFileSystem, mResourceManager);
  mParsers.push_back(std::move(gltfParser));
}
}  // namespace pd