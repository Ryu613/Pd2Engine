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

using MaterialIdType = u32;
using MaterialDefinitionHandle = MaterialIdType;
using MaterialInstanceHandle = MaterialIdType;
inline constexpr MaterialIdType invalidMaterialId = std::numeric_limits<MaterialIdType>::max();
}  // namespace pd