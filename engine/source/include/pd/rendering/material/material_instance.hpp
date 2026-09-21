#pragma once

#include "pd/rendering/material/material_definition.hpp"

namespace pd {
class MaterialInstance {
 public:
  using IdType = MaterialIdType;

  ~MaterialInstance() = default;
  DELETE_COPY(MaterialInstance);
  DEFAULT_MOVABLE(MaterialInstance);

 private:
  friend class MaterialManager;
  struct ParamValue {
    union Value {
      float f;
      float vec3[3];
      float vec4[4];
      Value()
          : vec4(0.0f, 0.0f, 0.0f, 0.0f) {}
    };
    Value value;
  };
  IdType mId{invalidMaterialId};
  IdType mDefinitionId{invalidMaterialId};
  std::vector<ParamValue> mParamValues;  // 与definition的顺序一致

  explicit MaterialInstance(MaterialDefinition* def);
};
}  // namespace pd