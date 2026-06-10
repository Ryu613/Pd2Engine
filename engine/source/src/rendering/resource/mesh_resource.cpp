#include "pd/rendering/resource/mesh_resource.hpp"

namespace pd {
MeshResource::MeshResource(Properties props) noexcept
    : mProperties(std::move(props)) {}

MeshResource::~MeshResource() {}

void MeshResource::doLoad() noexcept { }
void MeshResource::doUnload() noexcept {}
}  // namespace pd