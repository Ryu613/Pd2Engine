#pragma once

#include "pd/resource/resource_handle.hpp"
#include "pd/resource/resource/texture_resource.hpp"
#include "pd/resource/resource/mesh_resource.hpp"
#include "pd/resource/resource/material_resource.hpp"

namespace pd {
using TextureHandle = ResourceHandle<TextureResource_t>;
using MeshHandle = ResourceHandle<MeshResource_t>;
using MaterialHandle = ResourceHandle<MaterialResource>;
}  // namespace pd