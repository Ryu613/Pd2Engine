#include "pd/resource/resource/mesh_resource.hpp"

namespace pd {
MeshResource::MeshResource() noexcept { init(); }

MeshResource::~MeshResource() {}

void MeshResource::doLoad() noexcept {}
void MeshResource::doUnload() noexcept {}

void MeshResource::init() noexcept {}
}  // namespace pd