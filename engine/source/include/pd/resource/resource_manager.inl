#pragma once
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset.hpp"

namespace pd {
// general
// registerResourceFromAsset() : no specialization
template <typename TAsset, typename Tag>
inline Result<ResourceRef<Tag>> ResourceManager::registerResourceFromAsset(
    ResourceCreateInfo<TAsset> createInfo) noexcept {
  static_assert(false && "not supported asset or resource");
}

template <typename Tag>
inline Result<ResourceRef<Tag>> ResourceManager::acquire(ResourceHandle<Tag> handle) noexcept {
  retain(handle);
  return ResourceRef<Tag>{*this, handle};
}

template <DerivedResource TResource, typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::saveResource(std::unique_ptr<TResource> resource) noexcept {
  auto& pool = findPool<Tag>();
  auto handle = pool.emplace(std::move(*(resource.release())));
  if (!handle) {
    return make_error<ResourceHandle<Tag>>(ErrorCode::ResourceSaveFailed);
  }
  return handle;
}

template <typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::saveRegistry(ResourceHandle<Tag> handle) noexcept {
  PD_ASSERT(handle);
  auto resourceKey = getResourceKey<Tag>(handle);
  PD_ASSERT(resourceKey.keyId != invalidResourceId);

  auto& registry = findRegistry<Tag>();
  const auto it = registry.find(resourceKey);
  if (it != registry.end()) {
    LOG_ERROR("duplicate register! id:{}", handle.data.id);
    return make_error<ResourceHandle<Tag>>(ErrorCode::ResourceRegisterFailed);
  }
  ResourceEntry<Tag> newEntry{
      .refCount = 0,
      .loadCount = 0,
      .handle = handle,
  };
  auto [saveIt, isSuccess] = registry.emplace(resourceKey, newEntry);
  PD_ASSERT(isSuccess);
  return {};
}

template <typename Tag>
inline typename TagInfo<Tag>::ResourceType* ResourceManager::get(ResourceHandle<Tag> handle) noexcept {
  if (!handle) {
    return nullptr;
  }
  auto& dataPool = findPool<Tag>();

  using ResourceType = TagInfo<Tag>::ResourceType;

  ResourceType* resource = dataPool.get(handle);
  if (resource) {
    return resource;
  }
  return static_cast<ResourceType*>(nullptr);
}

template <typename Tag>
inline auto& ResourceManager::findPool() noexcept {
  if constexpr (std::is_same_v<Tag, TextureResource_t>) {
    return mTextures;
  } else if constexpr (std::is_same_v<Tag, BufferResource_t>) {
    return mBuffers;
  } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
    return mMeshes;
  } else if constexpr (std::is_same_v<Tag, PrefabResource_t>) {
    return mPrefabs;
  } else {
    static_assert(false, "resource type not supported!");
  }
}

template <typename Tag>
inline auto& ResourceManager::findRegistry() noexcept {
  if constexpr (std::is_same_v<Tag, TextureResource_t>) {
    return mTextureRegistry;
  } else if constexpr (std::is_same_v<Tag, BufferResource_t>) {
    return mBufferRegistry;
  } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
    return mMeshRegistry;
  } else if constexpr (std::is_same_v<Tag, PrefabResource_t>) {
    return mPrefabRegistry;
  } else {
    static_assert(false, "resource type not supported!");
  }
}

template <typename Tag>
[[nodiscard]]
inline std::optional<ResourceHandle<Tag>> ResourceManager::findRegisteredResource(const ResourceKey& key) noexcept {
  auto& registry = findRegistry<Tag>();
  auto it = registry.find(key);
  if (it != registry.end()) {
    return it.value().handle;
  }
  return std::nullopt;
}

template <typename Tag>
inline Result<void> ResourceManager::retain(ResourceHandle<Tag> handle) noexcept {
  auto* resourceEntry = findEntry(handle);
  if (!resourceEntry) {
    return make_error<void>(ErrorCode::ResourceNotFound);
  }
  resourceEntry->refCount++;
  return {};
}

template <typename Tag>
inline Result<void> ResourceManager::release(ResourceHandle<Tag> handle) noexcept {
  if (!handle) {
    return {};
  }
  auto* resource = get<Tag>(handle);
  if (!resource) {
    return {};
  }
  auto* resourceEntry = findEntry(handle);
  if (!resourceEntry) {
    return make_error<void>(ErrorCode::ResourceNotFound);
  }
  resourceEntry->refCount--;
  return {};
}

template <typename Tag>
inline Result<void> ResourceManager::load(ResourceHandle<Tag> handle) noexcept {
  if (auto res = loadInternal(handle); !res) {
    return res;
  }
  // auto resourceKey = getResourceKey(handle);
  // PD_ASSERT(resourceKey);

  // mRequestedRoots.emplace(resourceKey);
  return {};
}

template <typename Tag>
inline Result<void> ResourceManager::loadInternal(ResourceHandle<Tag> handle) noexcept {
  if (!handle) {
    return {};
  }
  auto* resource = get<Tag>(handle);
  if (!resource) {
    return {};
  }
  auto* resourceEntry = findEntry(handle);
  if (!resourceEntry) {
    return {};
  }
  auto loadContex = Resource::LoadContext{*this, mBackend, resourceEntry->loadCount};
  return resource->load(loadContex);
}

template <typename Tag>
inline Result<void> ResourceManager::unload(ResourceHandle<Tag> handle) noexcept {
  if (auto res = unloadInternal(handle); !res) {
    return res;
  }
  return {};
}

template <typename Tag>
inline Result<void> ResourceManager::unloadInternal(ResourceHandle<Tag> handle) noexcept {
  if (!handle) {
    return {};
  }
  auto* resourceEntry = findEntry(handle);
  if (!resourceEntry || resourceEntry->loadCount == 0) {
    return {};
  }
  auto* resource = get<Tag>(handle);
  if (!resource) {
    return {};
  }
  auto loadContex = Resource::LoadContext{*this, mBackend, resourceEntry->loadCount};
  return resource->unload(loadContex);
}

template <typename Tag>
void ResourceManager::unloadAll(bool force) noexcept {
  auto& registry = findRegistry<Tag>();
  for (auto it = registry.begin(); it != registry.end(); ++it) {
    auto handle = it.value().handle;
    auto* resource = get(handle);
    if (resource) {
      if (force) {
        it.value().loadCount = 0;
      }
      auto loadContext = Resource::LoadContext{
          .mgr = *this,
          .backend = mBackend,
          .loadCount = it.value().loadCount,
      };
      std::ignore = resource->unload(loadContext);
    }
  }
}

template <typename Tag>
inline ResourceManager::ResourceEntry<Tag>* ResourceManager::findEntry(ResourceHandle<Tag> handle) noexcept {
  auto resourceKey = getResourceKey<Tag>(handle);
  if (!resourceKey) return nullptr;
  auto& registry = findRegistry<Tag>();
  auto it = registry.find(resourceKey);
  if (it == registry.end()) {
    LOG_WARN("entry not found! id:{}", handle.data.id);
    return nullptr;
  }
  return &it.value();
}

template <typename Tag>
inline ResourceManager::ResourceKey ResourceManager::getResourceKey(ResourceHandle<Tag> handle) noexcept {
  PD_ASSERT(handle);
  auto* resource = get<Tag>(handle);
  PD_ASSERT(resource);
  const auto poolType = getPoolType<Tag>();
  return ResourceKey::makeKey(resource->sourceId(), poolType, resource->variantId());
}

template <typename Tag>
inline std::type_index ResourceManager::getPoolType() const noexcept {
  return std::type_index(typeid(Pool<typename TagInfo<Tag>::ResourceType, Tag>));
}

// template specialization
// gltf -> prefab
template <>
Result<ResourceRef<PrefabResource_t>> ResourceManager::registerResourceFromAsset(
    ResourceCreateInfo<GltfAsset> createInfo) noexcept;
}  // namespace pd