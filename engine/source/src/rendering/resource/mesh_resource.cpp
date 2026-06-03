#include "pd/rendering/resource/mesh_resource.hpp"

namespace pd {
MeshResource::MeshResource(Properties props) noexcept
    : mProperties(std::move(props)) {}

MeshResource::~MeshResource() {}

MeshResource::Status MeshResource::doLoad() noexcept { return Status::Loaded; }
MeshResource::Status MeshResource::doUnload() noexcept { return Status::Unload; }
}  // namespace pd