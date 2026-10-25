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

template <typename Tag>
struct TagInfo {
  using ResourceType = std::nullopt_t;
};
}  // namespace pd