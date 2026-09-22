#pragma once

#include "pd/core/utils/handle.hpp"

namespace pd {

using ResourceIdType = u32;
inline constexpr ResourceIdType invalidResourceId = std::numeric_limits<ResourceIdType>::max();

template <typename T>
using ResourceHandle = TypedHandle<T>;

enum class ResourceStatus : u8 {
  Unload = 1,
  Loading = 2,
  Loaded = 3,
};

struct MeshResource_t;
struct TextureResource_t;
struct ShaderResource_t;

using MeshHandle = ResourceHandle<MeshResource_t>;
using TextureHandle = ResourceHandle<TextureResource_t>;
using ShaderHandle = ResourceHandle<ShaderResource_t>;
}  // namespace pd