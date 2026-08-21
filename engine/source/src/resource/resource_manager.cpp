#include "pd/resource/resource_manager.hpp"

#include "pd/asset/asset.hpp"

namespace pd {

void ResourceManager::initialize() noexcept {
  if (mInitialized) {
    return;
  }
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
    auto handle = mMeshData.insert(std::move(meshResource));
    // 1.3 更新注册表
    auto [insertIt, success] =
        mMeshRegistry.emplace(meshResourceIdStr, ResourceEntry<MeshResource_t>{handle, 1});
    PD_ASSERT_MSG(success, "mesh registry insert failed!");
  }
  // 2. 注册texture资源
  // todo
  return {};
}
}  // namespace pd
