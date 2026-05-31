#pragma once

#include <typeindex>
#include <unordered_map>
#include <filesystem>

#include "pd/core/allocators.hpp"
#include "pd/resource/handle.hpp"
#include "pd/resource/resource.hpp"
#include "pd/rendering/resource/texture_resource.hpp"

namespace pd {
/**
 * @brief 资源句柄，只有ResourceManager能用，其他用户只能查看
 */
template <DerivedResource T>
class ResourceHandle : public Handle {
 public:
  ResourceHandle() noexcept
      : Handle() {}

 private:
  friend class ResourceManager;

  explicit ResourceHandle(Handle::HandleId id) noexcept
      : Handle(id) {}
};

using TextureHandle = ResourceHandle<TextureResource>;
/**
 * @brief 负责资源的注册，加载，卸载，管理资源的生命周期
 *
 * TODO:
 * 本版本不区分资产和运行时的资源
 * 前者应该包括资产处理成引擎内资源的过程,后者是对处理好的资产生成1个或多个资源方便在运行时使用
 */
class ResourceManager {
 public:
  ResourceManager() noexcept = default;
  ~ResourceManager() = default;

  NO_COPY_MOVE(ResourceManager);

  /**
   * @brief 加载模型文件
   * @note 拆分模型文件，注册资源，返回Model
   *
   */
  // template <DerivedAsset T>
  // static Model* loadGltfModel(const std::string& resourceName,
  //                             const std::string& resourcePath) noexcept {
  //   // 1. 按类型构造Model
  //   // 2. 把资源注册到registry
  // }

  /**
   * @brief 通过文件路径声明需要的资源
   * @param resourceName 资源的名字
   * @param resourcePath 资源的路径
   */
  template <DerivedResource T>
  [[nodiscard]] ResourceHandle<T> require(const std::string& resourceName,
                                          const std::string& resourcePath) noexcept {
    // TODO(author): 目前把path就当成resource的id，逻辑上不同，实现上等效
    const auto& resourceId = resourcePath;
    // 判断resourcePath对应的文件是否存在
    std::filesystem::path filePath(resourcePath);
    if (!std::filesystem::exists(filePath) ||
        !std::filesystem::is_regular_file(filePath)) {
      return ResourceHandle<T>{};
    }
    // 判断此资源是否已注册(根据resourcePath)
    auto typeIndex = std::type_index(typeid(T));
    // 若T不存在会创建一个map,故此处可以这么写
    auto& resourceTypeMap = mRegistry[typeIndex];
    auto resourceIt = resourceTypeMap.find(resourceId);
    // 若已注册，引用计数+1. 返回对应的句柄
    if (resourceIt != resourceTypeMap.end()) {
      resourceIt->second.refCount++;
      return ResourceHandle<T>{resourceIt->first};
      // 若未注册，把其构造并返回句柄
    } else {
      T* t = mArena.make<T>(resourceId, resourceName, resourcePath);
      // 写到注册表，key为此resource, value为Resource*
      resourceTypeMap.emplace(resourceId, t);
      // 引用计数+1(若不存在会自动创建)
      resourceTypeMap[resourceId].refCount++;

      return ResourceHandle<T>(resourceId);
    }
  }

  template <DerivedResource T>
  bool hasResource(const Resource::IdType& resourceId) noexcept {
    auto typeIt = mRegistry.find(std::type_index(typeid(T)));
    if (typeIt == mRegistry.end()) {
      return false;
    }
    auto& resourceMap = typeIt->second;
    auto resourceIt = resourceMap.find(resourceId);
    return resourceIt != resourceMap.end();
  }

  template <typename T>
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

  template <DerivedResource T>
  Resource* getResource(const Resource::IdType& resourceId) noexcept {
    auto typeIt = mRegistry.find(std::type_index(typeid(T)));
    if (typeIt == mRegistry.end()) {
      return nullptr;
    }
    auto& resourceMap = typeIt->second;
    auto resourceIt = resourceMap.find(resourceId);
    return resourceIt == resourceMap.end() ? nullptr : resourceIt->second.resource;
  }
};
}  // namespace pd