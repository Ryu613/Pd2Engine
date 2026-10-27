#include "pd/resource/resource/buffer_resource.hpp"

namespace pd {

Result<void> BufferResource::doLoad(Backend& backend) noexcept {
  //   LOG_DEBUG("loading mesh resource: {}", name());
  //   mVertexBuffer = mBackend->createBuffer({
  //       .debugName = mMeshDesc.debugName,
  //       .usage = BufferUsage::VertexBuffer,
  //       .memoryUsage = MemoryUsage::GpuOnly,
  //       .deviceSize = mMeshDesc.dataSize,
  //   });
  //   PD_ASSERT(mVertexBuffer);
  //   mBackend->writeBuffer({
  //       .buffer = mVertexBuffer,
  //       .pData = mMeshDesc.pData,
  //       .deviceSize = mMeshDesc.dataSize,
  //       .offset = mMeshDesc.dataOffset,
  //   });
  //   mIndexBuffer = mBackend->createBuffer({
  //       .debugName = mIndexDesc.debugName,
  //       .usage = BufferUsage::IndexBuffer,
  //       .memoryUsage = MemoryUsage::GpuOnly,
  //       .deviceSize = mIndexDesc.dataSize,
  //   });
  //   PD_ASSERT(mIndexBuffer);
  //   mBackend->writeBuffer({
  //       .buffer = mIndexBuffer,
  //       .pData = mIndexDesc.pData,
  //       .deviceSize = mIndexDesc.dataSize,
  //       .offset = mIndexDesc.dataOffset,
  //   });
  return {};
}
Result<void> BufferResource::doUnload(Backend& backend) noexcept {
  //   LOG_DEBUG("unloading mesh resource: {}", name());
  //   if (mVertexBuffer) {
  //     mBackend->destroyBuffer(mVertexBuffer);
  //   }
  //   if (mIndexBuffer) {
  //     mBackend->destroyBuffer(mIndexBuffer);
  //   }
  return {};
}
}  // namespace pd