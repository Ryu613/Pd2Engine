#pragma once

#include "pd/resource/resource_types.hpp"

namespace pd {
class MeshProcessor {
 public:
  struct Input {
    std::vector<math::vec3> positions;
    std::vector<math::vec3> normals;
    std::vector<math::vec2> uvs;
    std::vector<math::vec3> colors;
    std::vector<u32> indices;
  };

  static MeshData::Primitive process(Input& input) noexcept;

 private:
  static std::vector<Vertex> interleave(const Input& input) noexcept;
};
}  // namespace pd