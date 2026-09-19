#pragma once

#include "pd/backend/backend_types.hpp"
#include "vk1_common.hpp"

namespace vk1 {
namespace util {
inline VkShaderStageFlagBits toVkShaderStageFlagBits(pd::ShaderStage stage) {
  switch (stage) {
    using enum pd::ShaderStage;
    case Vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case Fragment:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
      PD_ASSERT_MSG(false, "shader stage enum not supported!");
  }
}

inline VkBufferUsageFlags ToVkBufferUsage(pd::BufferUsage usage) noexcept {
  switch (usage) {
    using enum pd::BufferUsage;
    case VertexBuffer:
      return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    case IndexBuffer:
      return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    case UniformBuffer:
      return VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    default:
      PD_ASSERT_MSG(false, "buffer usage type error!");
  }
  return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}
inline VkSharingMode ToVkSharingMode(pd::SharingMode sharingMode) noexcept {
  switch (sharingMode) {
    using enum pd::SharingMode;
    case Exclusive:
      return VK_SHARING_MODE_EXCLUSIVE;
    case Concurrent:
      return VK_SHARING_MODE_CONCURRENT;
    default:
      PD_ASSERT_MSG(false, "buffer sharing mode error!");
  }
}
inline VmaMemoryUsage ToVkMemoryUsage(pd::MemoryUsage memUsage) noexcept {
  switch (memUsage) {
    using enum pd::MemoryUsage;
    case Unknow:
      return VMA_MEMORY_USAGE_UNKNOWN;
    case GpuOnly:
      return VMA_MEMORY_USAGE_GPU_ONLY;
    case CpuOnly:
      return VMA_MEMORY_USAGE_CPU_ONLY;
    case CpuToGpu:
      return VMA_MEMORY_USAGE_CPU_TO_GPU;
    case GpuToCpu:
      return VMA_MEMORY_USAGE_GPU_TO_CPU;
    case CpuCopy:
      return VMA_MEMORY_USAGE_CPU_COPY;
    case GpuLazilyAllocated:
      return VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
    default:
      PD_ASSERT_MSG(false, "memory usage error!");
  }
}
}  // namespace util
}  // namespace vk1