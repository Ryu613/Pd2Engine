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
  struct Aabb {};
  struct Properties {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
  };

  explicit MeshResource(Properties props) noexcept;
  ~MeshResource();
  MOVABLE_ONLY(MeshResource);

 protected:
  void doLoad() noexcept override;
  void doUnload() noexcept override;

 private:
  Properties mProperties;
};
}  // namespace pd