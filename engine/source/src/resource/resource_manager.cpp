#include "pd/resource/resource_manager.hpp"

#include "pd/asset/asset.hpp"

namespace pd {
ResourceManager::ResourceManager(Backend* backend)
    : mBackend(backend) {}

ResourceManager::~ResourceManager() {}

Result<void> ResourceManager::init() noexcept { return {}; }

Result<void> ResourceManager::destroy() noexcept { return {}; }

Result<std::unique_ptr<PrefabResource>> ResourceManager::createGltfResource(ResourceIdType newId,
                                                                            const std::string& resourceName,
                                                                            GltfAsset* gltfAsset) noexcept {
  LOG_INFO("create gltf resource {}", resourceName);
  auto prefabRsc =
      std::unique_ptr<PrefabResource>(new PrefabResource(newId, gltfAsset->id(), resourceName, *mBackend, *this));
  const auto& meshData = gltfAsset->meshes();
  auto& prefabMeshes = prefabRsc->getMeshes();
  prefabMeshes.reserve(meshData.size());
  for (size_t meshIndex = 0; meshIndex < meshData.size(); ++meshIndex) {
    const auto& meshDataInfo = meshData[meshIndex].dataInfo;
    const auto& subMeshes = meshData[meshIndex].subMeshes;
    for (size_t subMeshIndex = 0; subMeshIndex < subMeshes.size(); ++subMeshIndex) {
      const auto& subMesh = subMeshes[subMeshIndex];
      auto newMeshId = nextId<MeshResource_t>();
      std::string subResourceName = std::format("{}:{}", resourceName, subMesh.name);
      auto meshRsc = std::unique_ptr<MeshResource>(
          new MeshResource(newMeshId, meshDataInfo.dataId, std::move(subResourceName), *mBackend));
      // 由asset mgr保证指针生命周期
      meshRsc->setMeshDesc({
          .debugName = std::format("vertex buffer for mesh: {}", subMesh.name),
          .pData = static_cast<const void*>(subMesh.vertices.data()),
          .dataSize = sizeof(subMesh.vertices[0]) * subMesh.vertices.size(),
          .dataOffset = 0,
      });
      meshRsc->setIndexData({
          .debugName = std::format("index buffer for mesh: {}", subMesh.name),
          .pData = static_cast<const void*>(subMesh.indices.data()),
          .dataSize = sizeof(subMesh.indices[0]) * subMesh.indices.size(),
          .dataOffset = 0,
          .indexCount = static_cast<u32>(subMesh.indices.size()),
      });
      auto meshRegRes = registerResource<MeshResource_t>(std::move(meshRsc));
      if (!meshRegRes) {
        return make_error<std::unique_ptr<PrefabResource>>(meshRegRes.error().code);
      }

      prefabMeshes.push_back(meshRegRes.value());
    }
  }
  // todo: material & texture resources
  return prefabRsc;
}

Result<std::unique_ptr<ShaderResource>> ResourceManager::createShaderResource(ResourceIdType newId,
                                                                              const std::string& resourceName,
                                                                              ShaderAsset* shaderAsset) noexcept {
  LOG_INFO("create shader resource {}", resourceName);
  auto shaderRsc =
      std::unique_ptr<ShaderResource>(new ShaderResource(newId, shaderAsset->id(), resourceName, *mBackend));

  // 由asset mgr保证原始数据生命周期
  shaderRsc->setSource(shaderAsset->sources());

  return shaderRsc;
}
}  // namespace pd