#pragma once

#include "pd/backend/backend_types.hpp"
#include "vk1_common.hpp"

namespace vk1 {
namespace util {

template <typename VkFlagsT, typename EnumT, typename ConvertFn>
inline VkFlagsT toVkFlags(std::span<const EnumT> values, ConvertFn&& convert) {
  VkFlagsT result = 0;
  for (const auto v : values) {
    result |= static_cast<VkFlagsT>(convert(v));
  }
  return result;
}

inline VkShaderStageFlagBits toVkShaderStageFlagBits(pd::ShaderStage stage) {
  switch (stage) {
    using enum pd::ShaderStage;
    case Vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case Geometry:
      return VK_SHADER_STAGE_GEOMETRY_BIT;
    case Fragment:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    case Compute:
      return VK_SHADER_STAGE_COMPUTE_BIT;
    case AllGraphics:
      return VK_SHADER_STAGE_ALL_GRAPHICS;
    case All:
      return VK_SHADER_STAGE_ALL;
  }
  PD_ASSERT_MSG(false, "shader stage enum not supported!");
  return {};
}

inline VkShaderStageFlags toVkShaderStageFlagBits(std::span<const pd::ShaderStage> stages) {
  return toVkFlags<VkShaderStageFlags>(stages, [](pd::ShaderStage s) { return toVkShaderStageFlagBits(s); });
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

inline VkCullModeFlags toVkCullMode(pd::CullMode cullMode) noexcept {
  switch (cullMode) {
    using enum pd::CullMode;
    case Front:
      return VK_CULL_MODE_FRONT_BIT;
    case Back:
      return VK_CULL_MODE_BACK_BIT;
    default:
      PD_ASSERT_MSG(false, "cull mode error!");
  }
}

inline VkFrontFace toVkFrontFace(pd::FrontFace frontFace) noexcept {
  switch (frontFace) {
    using enum pd::FrontFace;
    case Clockwise:
      return VK_FRONT_FACE_CLOCKWISE;
    case CounterClockwise:
      return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default:
      PD_ASSERT_MSG(false, "cull mode error!");
  }
}

inline VkImageUsageFlags toVkImageUsage(pd::TextureUsage usage) noexcept {
  switch (usage) {
    using enum pd::TextureUsage;
    case DefaultUsage:
      return VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    default:
      PD_ASSERT_MSG(false, "texture usage not supported yet!");
  }
}

inline VkFormat toVkFormat(pd::Format format) noexcept {
  switch (format) {
    using enum pd::TextureFormat;
    case RGBA8Unorm:
      return VK_FORMAT_R8G8B8A8_UNORM;
    case RGBA8SRGB:
      return VK_FORMAT_R8G8B8A8_SRGB;
    case RGB32Sfloat:
      return VK_FORMAT_R32G32B32_SFLOAT;
    case RG32Sfloat:
      return VK_FORMAT_R32G32_SFLOAT;
    case Undefined:
      return VK_FORMAT_UNDEFINED;
  }
  PD_ASSERT_MSG(false, "texture format not supported yet!");
  return {};
}

inline VkVertexInputRate toVkInputRate(pd::VertexRate inputRate) noexcept {
  switch (inputRate) {
    using enum pd::VertexRate;
    case Vertex:
      return VK_VERTEX_INPUT_RATE_VERTEX;
    case Instance:
      return VK_VERTEX_INPUT_RATE_INSTANCE;
  }
  PD_ASSERT_MSG(false, "vertex input rate not supported yet!");
  return {};
}

inline VkDescriptorType toVkDescriptorType(pd::ResourceBindingType bindingType) noexcept {
  switch (bindingType) {
    using enum pd::ResourceBindingType;
    case Sampler:
      return VK_DESCRIPTOR_TYPE_SAMPLER;
    case CombinedImageSampler:
      return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case SampledImage:
      return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case StorageImage:
      return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case UniformTexelBuffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case StorageTexelBuffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case UniformBuffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case StorageBuffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DynamicUniformBuffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case DynamicStorageBuffer:
      return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case InputAttachment:
      return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  }
  PD_ASSERT_MSG(false, "descriptor type not supported!");
  return {};
}

inline VkDescriptorBindingFlags toVkDescriptorBindingFlag(pd::ResourceBindingFlag flag) noexcept {
  switch (flag) {
    using enum pd::ResourceBindingFlag;
    case UpdateAfterBind:
      return VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
    case UpdateUnusedWhilePending:
      return VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
    case PartiallyBound:
      return VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    case VariableBindGroupEntriesCount:
      return VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
  }
  PD_ASSERT_MSG(false, "descriptor binding flag not supported!");
  return {};
}

inline VkDescriptorBindingFlags toVkDescriptorBindingFlagBits(std::span<const pd::ResourceBindingFlag> flags) {
  return toVkFlags<VkDescriptorBindingFlags>(flags,
                                             [](pd::ResourceBindingFlag f) { return toVkDescriptorBindingFlag(f); });
}
}  // namespace util
}  // namespace vk1