#pragma once

#include "pd/core/utils/handle.hpp"

namespace pd {

using ResourceIdType = u64;
inline constexpr ResourceIdType invalidResourceId = std::numeric_limits<ResourceIdType>::max();
using ResourceVariantIdType = u32;
inline constexpr ResourceVariantIdType defaultVariantId = std::numeric_limits<ResourceVariantIdType>::max();

enum class ResourceStatus : u8 {
  Unload = 1,
  Loading = 2,
  Loaded = 3,
};

// 高 4 位 = source，低 60 位 = raw id
inline constexpr unsigned sourceTypeShift = 60;
constexpr uint64_t sourceMask = 0xFULL << sourceTypeShift;  // 0xF000000000000000
constexpr uint64_t rawIdMask = ~sourceMask;                 // 0x0FFFFFFFFFFFFFFF

enum class ResourceSourceType : u8 {
  Invalid = 0,  // 保留：全 0 的 id 视为无效 id
  Asset = 1,    // 从 asset 创建（离线/编辑器烘焙出来的）
  Runtime = 2,  // 运行时动态创建（比如 RTT、动态贴图、临时 buffer）
};

// base resource types
struct BufferResource_t;
struct MeshResource_t;
struct TextureResource_t;
struct PrefabResource_t;

template <typename Tag>
using ResourceHandle = TypedHandle<Tag>;

using BufferHandle = ResourceHandle<BufferResource_t>;
using MeshHandle = ResourceHandle<MeshResource_t>;
using TextureHandle = ResourceHandle<TextureResource_t>;
using PrefabHandle = ResourceHandle<PrefabResource_t>;

inline u64 makeSourceId(ResourceSourceType sourceType, u64 rawId = invalidResourceId) noexcept {
  static std::atomic<u64> runtimeId{0};
  if (rawId == invalidResourceId) {
    return (static_cast<u64>(sourceType) << sourceTypeShift) | ++runtimeId;
  }
  return (static_cast<u64>(sourceType) << sourceTypeShift) | rawId;
}

inline u64 GetRawId(u64 sourceId) { return sourceId & rawIdMask; }

inline ResourceSourceType GetSourceType(u64 sourceId) {
  return static_cast<ResourceSourceType>(sourceId >> sourceTypeShift);
}

template <typename Tag>
struct TagInfo {
  using ResourceType = std::nullopt_t;
};

}  // namespace pd