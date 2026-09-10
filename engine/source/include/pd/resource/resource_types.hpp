#pragma once

#include "pd/core/utils/handle.hpp"
#include "pd/backend/backend_types.hpp"

namespace pd {
template <typename T>
using ResourceHandle = TypedHandle<T>;

enum class ResourceStatus : u8 {
  Unload = 1,
  Loading = 2,
  Loaded = 3,
};

struct MeshResource_t;
struct TextureResource_t;

using MeshHandle = ResourceHandle<MeshResource_t>;
using TextureHandle = ResourceHandle<TextureResource_t>;
}  // namespace pd