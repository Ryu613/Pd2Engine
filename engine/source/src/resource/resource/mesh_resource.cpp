#include "pd/resource/resource/mesh_resource.hpp"

#include "pd/backend/backend.hpp"

namespace pd {
MeshResource::MeshResource(Properties props) noexcept
    : mProps(props) {}

MeshResource::~MeshResource() {}

void MeshResource::doLoad(IBackend& backend) noexcept {
  LOG_DEBUG("loading mesh: {}", this->id());
  // 1. convert resource data to memory friendly form
  MeshOutput output = outputData();
  // 2. vertex buffer create & upload
  auto vertexBufferSize = getVertexDataSize();
  HwBuffer hwBufferVertex{
      .usage = BufferUsage::VertexBuffer,
      .deviceSize = vertexBufferSize,
  };
  hwBufferVertex.label = "vertex_buffer";
  mVertexBuffer = backend.createBuffer(hwBufferVertex);
  // 3. upload vertex buffer data
  BufferWriteOptions vertexWriteOptions{
      .dstBuffer = mVertexBuffer,
      .byteSize = vertexBufferSize,
      .pData = output.vertices.data(),
  };
  backend.writeBuffer(vertexWriteOptions);

  // 4. index buffer create
  auto indexBufferSize = getIndexDataSize();
  HwBuffer hwBufferIndex{
      .usage = BufferUsage::IndexBuffer,
      .deviceSize = indexBufferSize,
  };
  hwBufferIndex.label = "index_buffer";
  mIndexBuffer = backend.createBuffer(hwBufferIndex);
  // 5. update index buffer data
  BufferWriteOptions indexWriteOptions{
      .dstBuffer = mIndexBuffer,
      .byteSize = indexBufferSize,
      .pData = output.indices.data(),
  };
  backend.writeBuffer(indexWriteOptions);
}
void MeshResource::doUnload(IBackend& backend) noexcept {
  backend.destroyBuffer(mVertexBuffer);
  backend.destroyBuffer(mIndexBuffer);
}

MeshOutput MeshResource::outputData() noexcept {
  MeshOutput output;
  output.primitives.resize(mPrimitives.size());
  uint64_t indexOffset = 0;
  for (size_t i = 0; i < mPrimitives.size(); ++i) {
    auto primitiveIndexCount = mPrimitives[i].indices.size();
    output.primitives[i].indexCount = primitiveIndexCount;
    indexOffset += primitiveIndexCount;
    output.primitives[i].indexOffset = indexOffset;
    output.vertices.insert(output.vertices.end(), mPrimitives[i].vertices.begin(),
                           mPrimitives[i].vertices.end());
    output.indices.insert(output.indices.end(), mPrimitives[i].indices.begin(),
                          mPrimitives[i].indices.end());
  }
  return output;
}

void MeshResource::draw(IBackend* backend) noexcept {
  backend->drawIndexed(mVertexBuffer, mIndexBuffer);
}

uint64_t MeshResource::getVertexDataSize() noexcept {
  uint64_t size = 0;
  for (size_t i = 0; i < mPrimitives.size(); ++i) {
    size += sizeof(Vertex) * mPrimitives[i].vertices.size();
  }
  return size;
}
uint64_t MeshResource::getIndexDataSize() noexcept {
  uint64_t size = 0;
  for (size_t i = 0; i < mPrimitives.size(); ++i) {
    size += sizeof(uint32_t) * mPrimitives[i].indices.size();
  }
  return size;
}
}  // namespace pd