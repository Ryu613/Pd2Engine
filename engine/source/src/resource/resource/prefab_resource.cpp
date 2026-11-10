#include "pd/resource/resource/prefab_resource.hpp"

#include "pd/resource/resource_ref.inl"

namespace pd {

Result<void> PrefabResource::doLoad(LoadContext& ctx) noexcept {
  LOG_INFO("loading prefab resource: name={}", name());
  for (auto& eachMesh : mMeshes) {
    auto loadRes = eachMesh.load();
    if (!loadRes) {
      LOG_ERROR("resource id: {} load failed", eachMesh.handle().data.id);
      return make_error<void>(ErrorCode::ResourceLoadFailed);
    }
  }
  // todo: textures loading
  return {};
}
Result<void> PrefabResource::doUnload(LoadContext& ctx) noexcept {
  LOG_INFO("unloading prefab resource: name={}", name());
  // unload不应该断掉引用，没有实际动作

  return {};
}
}  // namespace pd