#pragma once

#include "pd/backend/backend_types.hpp"
#include "pd/core/utils/map.hpp"
#include "../vk1_device.hpp"

#include "../vk1_pipeline_layout.hpp"
#include "../vk1_pipeline.hpp"

namespace vk1 {
class ResourceRegistry {
 public:
  template <typename T>
  using Handle = pd::HwHandle<T>;

  ResourceRegistry(Vk1Device& device);
  ~ResourceRegistry();
  ResourceRegistry(const ResourceRegistry&) = delete;
  ResourceRegistry& operator=(const ResourceRegistry&) = delete;
  ResourceRegistry(ResourceRegistry&& rhs) noexcept = delete;
  ResourceRegistry& operator=(ResourceRegistry&& rhs) noexcept = delete;

  void init() noexcept;
  void destroy() noexcept;

  pd::HwPipelineLayoutHandle createPipelineLayout(const pd::PipelineLayoutDesc& desc) noexcept;
  void destroyPipelineLayout(pd::HwPipelineLayoutHandle) noexcept;
  pd::HwGraphicsPipelineHandle createGraphicsPipeline(pd::HwPipelineLayoutHandle handle, const pd::GraphicsPipelineDesc& desc) noexcept;
  void destroyGraphicsPipeline(pd::HwGraphicsPipelineHandle) noexcept;

  template <typename Tag>
  auto* getResource(Handle<Tag> handle) noexcept;

  template <typename Tag>
  auto& findPool() noexcept;

 private:
  template <typename Tag, typename T>
  struct Slot {
    uint32_t gen = 0;
    T resource;
  };

  template <typename Tag>
  uint32_t nextId() noexcept;

  template <typename T>
  void setObjectName(VkDevice device, VkObjectType type, T handle, std::string_view name) noexcept;

  Vk1Device* mDevice = nullptr;

  template <typename Tag, typename T>
  using DataPool = pd::util::RobinMap<pd::HandleIdType, Slot<Tag, T>>;

  DataPool<pd::PipelineLayout_t, Vk1PipelineLayout> mPipelineLayouts;
  DataPool<pd::GraphicsPipeline_t, Vk1Pipeline> mGraphicsPipelines;
};

template <typename Tag>
inline uint32_t ResourceRegistry::nextId() noexcept {
  static uint32_t tagId;
  return tagId++;
}

template <typename T>
inline void ResourceRegistry::setObjectName(VkDevice device, VkObjectType type, T handle,
                                            std::string_view name) noexcept {
#if VK_EXT_debug_utils
  if (device == VK_NULL_HANDLE || vkSetDebugUtilsObjectNameEXT == nullptr || name.empty()) {
    return;
  }
  const VkDebugUtilsObjectNameInfoEXT nameInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .objectType = type,
      .objectHandle = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(handle)),
      .pObjectName = name.data(),
  };
  vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
#endif
}

template <typename Tag>
inline auto& ResourceRegistry::findPool() noexcept {
  if constexpr (std::is_same_v<Tag, pd::PipelineLayout_t>) {
    return mPipelineLayouts;
  } else if (std::is_same_v<Tag, pd::GraphicsPipeline_t>) {
    return mGraphicsPipelines;
  }
  PD_ASSERT_MSG(false, "vulkan resource pool error: not supported tag");
}

template <typename Tag>
inline auto* ResourceRegistry::getResource(Handle<Tag> handle) noexcept {
  auto& dataPool = findPool<Tag>();
  auto it = dataPool.find(handle.data.id);
  if (it != dataPool.end()) {
    // todo: gen equality
    return &(it->second.resource);
  }
  using ResourcePtr = decltype(&(it->second.resource));
  return static_cast<ResourcePtr>(nullptr);
}
}  // namespace vk1