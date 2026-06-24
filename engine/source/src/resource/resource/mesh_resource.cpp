#include "pd/resource/resource/mesh_resource.hpp"

#include "pd/backend/backend.hpp"

namespace pd {
MeshResource::MeshResource(Properties props) noexcept
    : mProps(props) {}

MeshResource::~MeshResource() {}

void MeshResource::doLoad(IBackend& backend) noexcept {
  log::debug("loading mesh: {}", this->id());
  auto bufferSize = getVertexDataSize();
  HwBuffer hwBuffer{
      .usage = BufferUsage::VertexBuffer,
      .deviceSize = bufferSize,
  };
  hwBuffer.label = "vertex_buffer";

  mVertexBuffer = backend.createBuffer(hwBuffer);
  BufferWriteOptions writeOptions{
      .dstBuffer = mVertexBuffer,
      .byteSize = bufferSize,
  };
  backend.writeBuffer(writeOptions);
}
void MeshResource::doUnload(IBackend& backend) noexcept {
  backend.destroyBuffer(mVertexBuffer);
  // backend.destroyBuffer(mIndexBuffer);
}

uint64_t MeshResource::getVertexDataSize() noexcept {
  uint64_t size = 0;
  for (const auto& primitive : mPrimitives) {
    size += sizeof(Vertex) * primitive.vertices.size();
  }
  return size;
}
}  // namespace pd