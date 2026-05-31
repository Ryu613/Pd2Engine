#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class MeshResource : public Resource {
 public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
  };
  struct Properties {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint32_t> indices;
  };
};
}  // namespace pd