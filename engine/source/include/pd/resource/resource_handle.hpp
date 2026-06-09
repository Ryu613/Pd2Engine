#pragma once

#include "pd/core/handle.hpp"
#include "pd/rendering/resource/texture_resource.hpp"
#include "pd/rendering/resource/mesh_resource.hpp"
#include "pd/rendering/resource/material_resource.hpp"

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
using MeshHandle = ResourceHandle<MeshResource>;
using MaterialHandle = ResourceHandle<MaterialResource>;
}  // namespace pd