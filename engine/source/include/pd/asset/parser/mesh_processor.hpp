#pragma once

#include "pd/resource/resource/mesh_resource.hpp"

namespace pd {
class MeshProcessor {
 public:
  struct Input {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> colors;
    std::vector<uint32_t> indices;
  };

  static MeshResource::Properties process(Input& input) noexcept;

 private:
  static std::vector<MeshResource::Vertex> interleave(const Input& input) noexcept;
};
}  // namespace pd