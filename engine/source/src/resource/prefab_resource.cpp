#include "pd/resource/prefab_resource.hpp"

#include "pd/resource/resource_manager.hpp"

namespace pd {
PrefabResource::PrefabResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend,
                               ResourceManager& rscMgr)
    : Resource(id, assetId, name),
      mBackend(&backend),
      mResourceMgr(&rscMgr) {}

PrefabResource::~PrefabResource() {}

Result<void> PrefabResource::doLoad() noexcept {
  LOG_INFO("loading prefab resource:id={},name={}", id(), name());
  for (auto eachMesh : mMeshHandles) {
    auto loadRes = mResourceMgr->loadResource(eachMesh);
    if (!loadRes) {
      LOG_ERROR("resource id: {} load failed", eachMesh.data.id);
      return make_error<void>(ErrorCode::ResourceLoadFailed);
    }
  }
  // todo: textures loading
  return {};
}
Result<void> PrefabResource::doUnload() noexcept {
  LOG_INFO("unloading prefab resource:id={},name={}", id(), name());

  // 倒着卸载
  if (!mMeshHandles.empty()) {
    for (size_t i = mMeshHandles.size(); i-- > 0;) {
      auto loadRes = mResourceMgr->unloadResource(mMeshHandles[i]);
      if (!loadRes) {
        LOG_ERROR("resource id: {} unload failed", mMeshHandles[i].data.id);
        return make_error<void>(ErrorCode::ResourceUnLoadFailed);
      }
    }
  }

  // todo: textures

  return {};
}
}  // namespace pd