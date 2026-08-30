#include "pd/asset/parser/mesh_processor.hpp"

#include "meshoptimizer.h"

namespace pd {
MeshData::Primitive MeshProcessor::process(Input& input) noexcept {
  std::vector<Vertex> vertices = interleave(input);
  auto indices = std::move(input.indices);
  size_t vertexCount = vertices.size();
  bool hasIndices = !indices.empty();
  size_t indexCount = hasIndices ? indices.size() : vertexCount;

  // 优化顶点和索引数据
  // 1. 生成新的顶点映射表，用于记录旧顶点索引对应的新顶点索引
  std::vector<uint32_t> remap(vertexCount);
  size_t newVertexCount = meshopt_generateVertexRemap(
      remap.data(), hasIndices ? indices.data() : nullptr,
      hasIndices ? indices.size() : vertexCount, vertices.data(), vertexCount, sizeof(Vertex));

  indices.resize(indexCount);
  // 为新的索引数据分配空间
  meshopt_remapIndexBuffer(indices.data(), hasIndices ? indices.data() : nullptr,
                           hasIndices ? indices.size() : vertexCount, remap.data());
  // 为新的顶点数据分配空间
  meshopt_remapVertexBuffer(vertices.data(), vertices.data(), vertexCount, sizeof(Vertex),
                            remap.data());
  // 优化顶点数据
  meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());
  // 优化overdraw
  // 注意： 移动设备的tiled deferred rendering不会从中受益
  meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(), &vertices[0].pos.x,
                           vertices.size(), sizeof(Vertex), 1.05f);
  // 优化顶点属性读取时的内存局部性，会对顶点数据和相关的索引进行重排
  meshopt_optimizeVertexFetch(vertices.data(), indices.data(), indices.size(), vertices.data(),
                              vertices.size(), sizeof(Vertex));

  MeshData::Primitive primitive{
      // .firstIndex = 0,
  };
  return primitive;
}

std::vector<Vertex> MeshProcessor::interleave(const Input& input) noexcept {
  PD_ASSERT_MSG(input.positions.size() == input.normals.size(),
                "position should has same size as normals");
  std::vector<Vertex> vertices(input.positions.size());
  for (size_t i = 0; i < input.positions.size(); ++i) {
    vertices[i].pos = input.positions[i];
    vertices[i].normal = input.normals[i];
    vertices[i].uv = i < input.uvs.size() ? input.uvs[i] : glm::vec2{0.0f};
    // vertices[i].color = i < input.colors.size() ? input.colors[i] : glm::vec3{1.0f};
  }

  return vertices;
}
}  // namespace pd