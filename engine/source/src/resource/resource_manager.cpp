#include "pd/resource/resource_manager.hpp"

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
  // 1. 判重, 同一个资产只能注册一次，如果要生成独立的，需要给不同的variant
  const auto resourceKey = ResourceKey::fromAsset(assetId, createInfo.variantId);
  if (auto findResult = findRegisteredResource<PrefabResource_t>(resourceKey)) {
    return make_error<ResourceRef<PrefabResource_t>>(ErrorCode::ResourceRegisterFailed);
  }
  // 2. 创建资源(有ref但是refcount都是0，若中途失败，已成功的资源都是0，方便被后续回收)
  auto& asset = *createInfo.pAsset;

  // create prefab resource
  auto prefabResource =
      std::unique_ptr<PrefabResource>(new PrefabResource(assetId, createInfo.variantId, asset.info().name));
  auto prefabHandleResult = saveResource<PrefabResource, PrefabResource_t>(std::move(prefabResource));
  PD_ASSERT(prefabHandleResult);

  std::vector<ResourceHandle<MeshResource_t>> meshCache;
  meshCache.reserve(asset.meshes().size());
  std::vector<ResourceHandle<BufferResource_t>> bufferCache;
  bufferCache.reserve(asset.meshes().size() * 2);
  std::vector<ResourceHandle<TextureResource_t>> textureCache;
  textureCache.reserve(asset.textures().size());

  {
    // 从底层资源开始创建，最后再创建顶层资源
    // create mesh resource
    for (const auto& mesh : asset.meshes()) {
      // 1 mesh -> 2 buffers
      auto newMesh = std::unique_ptr<MeshResource>(new MeshResource(mesh.id, createInfo.variantId, mesh.name));

      u32 firstIndexValue = 0;
      i32 vertexOffsetValue = 0;
      newMesh->mSubMeshes.reserve(mesh.primitives.size());
      for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        auto& primitive = mesh.primitives[i];
        MeshResource::SubMesh eachSubMesh{
            .firstIndex = firstIndexValue,
            .indexCount = static_cast<u32>(primitive.indices.size()),
            .vertexOffset = vertexOffsetValue,
            .materialSlot = primitive.materialIndex,
            .vertexData = primitive.vertices,
            .indexData = primitive.indices,
        };
        firstIndexValue += primitive.indices.size();
        vertexOffsetValue += primitive.vertices.size();
        newMesh->mSubMeshes.emplace_back(std::move(eachSubMesh));
      }
      newMesh->mInfo.vertexCount = vertexOffsetValue;
      newMesh->mInfo.indexCount = firstIndexValue;
      auto newMeshResult = saveResource<MeshResource, MeshResource_t>(std::move(newMesh));
      if (!newMeshResult) {
        LOG_ERROR("mesh resource create failed! \n{}", newMeshResult.error().msg);
        return make_error<ResourceRef<PrefabResource_t>>(newMeshResult.error().code);
      }
      meshCache.push_back(newMeshResult.value());
      // 当前版本一个mesh对一个buffer resource,但是架构上允许多个buffer resource对应同一个rhi buffer handle
      // 当前版本buffer不可复用,一个mesh2个buffer
      auto newVertexBuffer = std::unique_ptr<BufferResource>(new BufferResource());
      newVertexBuffer->mInfo = {
          .size = mesh.verticesSize,
          .usage = BufferUsage::VertexBuffer,
      };
      auto newVBufferResult = saveResource<BufferResource, BufferResource_t>(std::move(newVertexBuffer));
      if (!newVBufferResult) {
        LOG_ERROR("buffer resource create failed! \n{}", newVBufferResult.error().msg);
        return make_error<ResourceRef<PrefabResource_t>>(newVBufferResult.error().code);
      }
      bufferCache.push_back(newVBufferResult.value());

      auto newIndexBuffer = std::unique_ptr<BufferResource>(new BufferResource());
      newIndexBuffer->mInfo = {
          .size = mesh.indicesSize,
          .usage = BufferUsage::IndexBuffer,
      };
      auto newIBufferResult = saveResource<BufferResource, BufferResource_t>(std::move(newIndexBuffer));
      if (!newIBufferResult) {
        LOG_ERROR("buffer resource create failed! \n{}", newIBufferResult.error().msg);
        return make_error<ResourceRef<PrefabResource_t>>(newIBufferResult.error().code);
      }
      bufferCache.push_back(newIBufferResult.value());

      // 建立引用关系(与存储操作分离)
      auto* prefabResource = get<PrefabResource_t>(prefabHandleResult.value());
      PD_ASSERT(prefabResource);
      auto* meshResource = get<MeshResource_t>(newMeshResult.value());
      PD_ASSERT(meshResource);
      meshResource->mVertexBuffer = ResourceRef<BufferResource_t>{*this, newVBufferResult.value()};
      meshResource->mIndexBuffer = ResourceRef<BufferResource_t>{*this, newIBufferResult.value()};

      prefabResource->addMesh({*this, newMeshResult.value()});
    }
    for (const auto& texture : asset.textures()) {
      // todo
    }
  }
  // 先写入注册表
  for (auto buffer : bufferCache) {
    if (auto res = saveRegistry(buffer); !res) {
      LOG_ERROR("update prefab's buffer ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  for (auto mesh : meshCache) {
    if (auto res = saveRegistry(mesh); !res) {
      LOG_ERROR("update prefab's mesh ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  for (auto texture : textureCache) {
    if (auto res = saveRegistry(texture); !res) {
      LOG_ERROR("update prefab's mesh ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  if (auto res = saveRegistry(prefabHandleResult.value()); !res) {
    LOG_ERROR("update prefab ref count error!{}", res.error().msg);
    return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
  }
  // 再更新引用计数
  for (auto buffer : bufferCache) {
    if (auto res = retain(buffer); !res) {
      LOG_ERROR("update prefab's buffer ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  for (auto mesh : meshCache) {
    if (auto res = retain(mesh); !res) {
      LOG_ERROR("update prefab's mesh ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  for (auto texture : textureCache) {
    if (auto res = retain(texture); !res) {
      LOG_ERROR("update prefab's mesh ref count error!{}", res.error().msg);
      return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
    }
  }
  if (auto res = retain(prefabHandleResult.value()); !res) {
    LOG_ERROR("update prefab ref count error!{}", res.error().msg);
    return make_error<ResourceRef<PrefabResource_t>>(res.error().code);
  }
  return {};
}

Result<void> ResourceManager::gc() noexcept {
  // 1. prefab
  // 2. mesh
  // 3. texture
  // 4. buffer

  return {};
}
}  // namespace pd