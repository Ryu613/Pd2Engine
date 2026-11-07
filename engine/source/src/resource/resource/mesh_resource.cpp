#include "pd/resource/resource/mesh_resource.hpp"

#include "pd/resource/resource_ref.inl"

namespace pd {

Result<void> MeshResource::doLoad(LoadContext& ctx) noexcept {
  if (auto res = mVertexBuffer.load(); !res) {
    return res;
  }
  if (auto res = mIndexBuffer.load(); !res) {
    return res;
  }
  for (const auto& submesh : mSubMeshes) {
    const void* pData = submesh.vertexData.data();
    auto vertexUploadRes = ctx.mgr.uploadBuffer(mVertexBuffer.handle(),                          // handle
                                                pData,                                           // data pointer
                                                sizeof(VertexData) * submesh.vertexData.size(),  // size
                                                sizeof(VertexData) * submesh.vertexOffset        // offset
    );
    if (!vertexUploadRes) {
      LOG_ERROR("mesh vertex data upload failed! resource:{}", sourceId());
      return make_error<void>(ErrorCode::ResourceLoadFailed);
    }
    auto indexUploadRes = ctx.mgr.uploadBuffer(mIndexBuffer.handle(),                   //
                                               submesh.indexData.data(),                //
                                               sizeof(u32) * submesh.indexData.size(),  //
                                               sizeof(u32) * submesh.firstIndex         //
    );
    if (!indexUploadRes) {
      LOG_ERROR("mesh index data upload failed! resource:{}", sourceId());
      return make_error<void>(ErrorCode::ResourceLoadFailed);
    }
  }
  return {};
}
Result<void> MeshResource::doUnload(LoadContext& ctx) noexcept {
  mVertexBuffer.reset();
  mIndexBuffer.reset();
  return {};
}
}  // namespace pd