#include "pd/resource/resource_manager.hpp"

#include "pd/asset/asset.hpp"

namespace pd {

void ResourceManager::initialize(IBackend* backend) noexcept {
  if (mInitialized) {
    return;
  }
  mBackend = backend;
  mInitialized = true;
}

void ResourceManager::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
}

Result<void> ResourceManager::loadAsset(Asset* pAsset) noexcept {
  if (!pAsset) {
    return make_error<void>(ErrorCode::ResourceLoadFailed);
  }
  // 1. 注册mesh资源
  for (const auto& pMesh : pAsset->getMeshes()) {
    // 1.1 检查mesh id是否已存在
    if (pMesh == nullptr || pMesh->id == u32_max) {
      return make_error<void>(ErrorCode::ResourceLoadFailed);
    }
    std::string meshResourceIdStr = std::format("mesh_{}_{}", pAsset->getId(), pMesh->id);
    auto it = mMeshRegistry.find(meshResourceIdStr);
    // 1.1.1 若存在，则更新计数
    if (it != mMeshRegistry.end()) {
      it.value().refCount++;
      continue;
    }
    // 1.2 若不存在，则生成mesh resource并注册
    MeshResource meshResource{meshResourceIdStr};
    meshResource.setRawData(pMesh.get());
    auto handle = mMeshData.emplace(std::move(meshResource));
    // 1.3 更新注册表
    auto [insertIt, success] =
        mMeshRegistry.emplace(meshResourceIdStr, ResourceEntry<MeshResource_t>{handle, 1});
    PD_ASSERT_MSG(success, "mesh registry insert failed!");
  }
  // 2. 注册texture资源
  // todo
  return {};
}

void ResourceManager::loadAll() noexcept {
  // 1. 加载mesh
  loadResources<MeshResource_t>();
  // 2. 加载texture
  loadResources<TextureResource_t>();
}

void ResourceManager::clearAll() noexcept {
  unloadResources<MeshResource_t>(true);
  unloadResources<TextureResource_t>(true);
  mMeshData.clear();
  mTextureData.clear();
  mMeshRegistry.clear();
  mTextureRegistry.clear();
}

void ResourceManager::gc() noexcept {}
}  // namespace pd
