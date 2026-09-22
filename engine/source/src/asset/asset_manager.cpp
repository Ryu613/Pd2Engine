#include "pd/asset/asset_manager.hpp"

#include "pd/asset/parser/gltf_parser.hpp"
#include "pd/asset/parser/shader_parser.hpp"

namespace pd {
AssetManager::AssetManager(IFileSystem* fs)
    : mFs(fs) {}

AssetManager::~AssetManager() {}

Result<void> AssetManager::init() noexcept {
  initParsers();
  return {};
}
Result<void> AssetManager::destroy() noexcept { return {}; }

Result<AssetHandle> AssetManager::createAsset(const Asset::CreateInfo& assetInfo) noexcept {
  // 检查文件是否存在，类型是否正确, 文件是否可读
  const auto& assetPath = assetInfo.path;
  auto& fs = *mFs;
  if (!fs.exists(assetPath) || !fs.isFile(assetPath)) {
    LOG_ERROR("asset path is illegal: {}", assetPath);
    return make_error<AssetHandle>(ErrorCode::AssetFileNotFound);
  }
  // 检查是否已存在此资产
  auto it = mRegistry.find(assetPath);
  if (it != mRegistry.end()) {
    return AssetHandle{it->second};
  }
  const auto newId = nextId();
  // 1 创建资产实例
  std::unique_ptr<Asset> newAsset;
  switch (assetInfo.parseType) {
    using enum AssetType;
    case Gltf:
      newAsset = std::unique_ptr<GltfAsset>(new GltfAsset(newId, assetInfo));
      break;
    case Shader:
      newAsset = std::unique_ptr<ShaderAsset>(new ShaderAsset(newId, assetInfo));
      break;
    default:
      PD_ASSERT_MSG(false, "asset type not supported!");
  }
  // 2. 生成资产实例
  auto parseResult = mParsers[static_cast<size_t>(assetInfo.parseType)]->parse(*newAsset);
  if (!parseResult) {
    return make_error<AssetHandle>(ErrorCode::AssetParseFailed);
  }
  // 3. 记录到资产记录表
  auto [regIt, regOk] = mRegistry.emplace(assetPath, newId);
  PD_ASSERT_MSG(regOk, "asset registry update error!");
  // 4. 记录到数据库
  auto [insIt, insOk] = mStorage.emplace(newId, std::move(newAsset));
  PD_ASSERT_MSG(insOk, "unexpected asset creations!");

  return AssetHandle{insIt->second->id()};
}

void AssetManager::initParsers() noexcept {
  mParsers.reserve(4);
  auto gltfParser = std::make_unique<GltfParser>(mFs);
  mParsers.push_back(std::move(gltfParser));
  auto shaderParser = std::make_unique<ShaderParser>(mFs);
  mParsers.push_back(std::move(shaderParser));
}

u32 AssetManager::nextId() noexcept { return ++mNextId; }
}  // namespace pd