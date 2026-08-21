#pragma once

#include "pd/core/math/math.hpp"

namespace pd {
struct Vertex {
  math::vec3 pos;
  math::vec3 normal;
  math::vec2 uv;
  // math::vec4 tangent;
  // todo(ryu613): skinned vertex data
};
struct Aabb {};

struct MeshData {
  u32 id = u32_max;
  std::vector<Vertex> vertices;
  std::vector<u32> indices;

  struct Primitive {
    u32 indexOffset = 0;
    u32 indexCount = 0;
    u32 materialIndex = u32_max;
  };
  std::vector<Primitive> primitives;
  Aabb boundingBox;
};

struct TextureInfo {
  std::string name;
  std::string path;
  u32 width = 0;
  u32 height = 0;
  u32 depth = 1;
  u32 channel = 4;
  u8 levels = 1;
  u8 samples = 1;
};
}  // namespace pd