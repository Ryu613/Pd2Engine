#pragma once

#include "pd/backend/backend_types.hpp"

namespace pd {
enum class ParamType : u8 {
  Vec3,
  Vec4,
  Float,
  Double,
};

enum class ShaderType : u8 {
  Vertex,
  Fragment,
};
enum class ShadingModel : u8 {
  Lit,
  Unlit,
};
}  // namespace pd