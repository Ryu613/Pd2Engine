#pragma once

#include <typeindex>
#include <unordered_map>

#include "pd/core/allocators.hpp"
#include "pd/resource/resource_handle.hpp"

namespace pd {
/**
 * @brief 负责资源的注册，加载，卸载，管理资源的生命周期
 *
 * TODO:
 * 本版本不区分资产和运行时的资源
 * 前者应该包括资产处理成引擎内资源的过程,后者是对处理好的资产生成1个或多个资源方便在运行时使用
 * handle id暂时采用自增，未采用分代+freelist
 */
class ResourceManager {
 public:
  ResourceManager() noexcept = default;
  ~ResourceManager() = default;

  NO_COPY_MOVE(ResourceManager);

  /**
   * @brief 运行时资源注册
   *
   * @tparam T 资源类型
   * @param t 具体资源实例
   * @return ResourceHandle<T> 资源句柄
   */
  template <BaseOfResource T>
  [[nodiscard]] ResourceHandle<T> registerResource(T& t) noexcept {
    // 为resource生成id
    t.mId = generateId();
    const auto& resourceId = t.getId();
    // 判断此资源是否已注册(根据resourcePath)
    auto typeIndex = std::type_index(typeid(T));
    // 若T不存在会创建一个map,故此处可以这么写
    auto& resourceTypeMap = mRegistry[typeIndex];
    auto resourceIt = resourceTypeMap.find(resourceId);
    // 若已注册，引用计数+1. 返回对应的句柄
    if (resourceIt != resourceTypeMap.end()) {
      resourceIt->second.refCount++;
      return ResourceHandle<T>{resourceIt->first};
      // 若未注册，把其注册并返回句柄
    }
    // 写到注册表，key为此resource, value为Resource*
    resourceTypeMap.emplace(resourceId, &t);
    // 引用计数+1(若不存在会自动创建)
    resourceTypeMap[resourceId].refCount++;

    return ResourceHandle<T>(resourceId);
  }

  template <BaseOfResource T>
  bool hasResource(const Resource::IdType& resourceId) noexcept {
    auto typeIt = mRegistry.find(std::type_index(typeid(T)));
    if (typeIt == mRegistry.end()) {
      return false;
    }
    auto& resourceMap = typeIt->second;
    auto resourceIt = resourceMap.find(resourceId);
    return resourceIt != resourceMap.end();
  }

  template <BaseOfResource T>
  void release(ResourceHandle<T> handle) noexcept {
    Resource* resource = getResource<T>(handle.getId());
    if (resource == nullptr) {
      return;
    }
    mRegistry.at(std::type_index(typeid(T))).at(handle.getId()).refCount--;
  }

  /**
   * @brief 清除所有资源，一般是在整体关闭时使用
   */
  void clearAll() noexcept;

  /**
   * @brief 检测资源引用记录并清除无引用的资源
   */
  void gc() noexcept;

 private:
  HeapAllocator mArena;
  uint32_t mCurrentId = 0;

  struct ResourceEntry {
    Resource* resource = nullptr;
    uint32_t refCount = 0;
  };
  // 两阶段存储,先按resource的类型查询，再按id查询
  // 只负责维护资源注册表,不维护引用计数
  using Registry =
      std::unordered_map<std::type_index,
                         std::unordered_map<Resource::IdType, ResourceEntry>>;
  Registry mRegistry;

  template <BaseOfResource T>
  Resource* getResource(const Resource::IdType& resourceId) noexcept {
    auto typeIt = mRegistry.find(std::type_index(typeid(T)));
    if (typeIt == mRegistry.end()) {
      return nullptr;
    }
    auto& resourceMap = typeIt->second;
    auto resourceIt = resourceMap.find(resourceId);
    return resourceIt == resourceMap.end() ? nullptr : resourceIt->second.resource;
  }

  [[nodiscard]] Resource::IdType generateId() noexcept { return mCurrentId++; }
};
}  // namespace pd