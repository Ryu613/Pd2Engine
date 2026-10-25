#include "pd/resource_new/resource_manager.hpp"

namespace pd {

ResourceManager::ResourceManager(Backend& backend)
    : mBackend(backend) {}

ResourceManager::~ResourceManager() {}

Result<void> ResourceManager::init() noexcept { return {}; }

Result<void> ResourceManager::destroy() noexcept { return {}; }

template <>
Result<ResourceRef<PrefabResource_t>> ResourceManager::registerResourceFromAsset(
    ResourceCreateInfo<GltfAsset> createInfo) noexcept {
  PD_ASSERT_MSG(createInfo.pAsset, "asset pointer is null!");
  auto assetId = createInfo.pAsset->id();
  // 1. 判重, 若有，返回handle
  const auto resourceKey = ResourceKey{
      .assetId = assetId,
      .variantId = createInfo.variantId,
  };
  if (auto handle = findRegisteredResource<PrefabResource_t>(resourceKey)) {
    return ResourceRef<PrefabResource_t>(*this);
  }
  // 2. 创建资源(有ref但是refcount都是0，若中途失败，已成功的资源都是0，可被后续回收)
  auto& asset = *createInfo.pAsset;
  auto prefabResource =
      std::unique_ptr<PrefabResource>(new PrefabResource(assetId, createInfo.variantId, asset.info().name));
  auto prefabHandle = saveResource<PrefabResource, PrefabResource_t>(std::move(prefabResource));
  PD_ASSERT(prefabHandle && prefabHandle.value());
  // 从底层资源开始创建，最后再创建顶层资源
  for (const auto& mesh : asset.meshes()) {
    auto newMesh = std::unique_ptr<MeshResource>(new MeshResource(mesh.id, createInfo.variantId, mesh.name));
    auto newMeshResult = saveResource<MeshResource, MeshResource_t>(std::move(newMesh));
    if (!newMeshResult) {
      LOG_ERROR("resource register failed: {}", newMeshResult.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(ErrorCode::ResourceRegisterFailed);
    }
    auto* newMeshResource = get<MeshResource_t>(newMeshResult.value());
    PD_ASSERT(newMeshResource);
    for (const auto& submesh : mesh.primitives) {
      // 
    }
  }
  // prefab引用关系建立
  retain(prefabHandle.value());
  // 5. 写入注册表
  return {};
}
}  // namespace pd