#include "pd/resource/resource/mesh_resource.hpp"

namespace pd {

Result<void> MeshResource::doLoad(Backend& backend) noexcept {
  if (auto res = mVertexBuffer.load(); !res) {
    return res;
  }
  if (auto res = mIndexBuffer.load(); !res) {
    return res;
  }
  // upload mesh data
  return {};
}
Result<void> MeshResource::doUnload(Backend& backend) noexcept {
  mVertexBuffer.reset();
  mIndexBuffer.reset();
  return {};
}
}  // namespace pd