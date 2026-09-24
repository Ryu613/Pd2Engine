#pragma once

#include "pd/rendering/material/material_types.hpp"

#include "pd/resource/resource_types.hpp"

namespace pd {
class MaterialDefinition {
 public:
  using IdType = MaterialIdType;

  ~MaterialDefinition() = default;
  DELETE_COPY(MaterialDefinition);
  DEFAULT_MOVABLE(MaterialDefinition);

 private:
  friend class Builder;
  struct ParamDef {
    std::string name;
    ParamType type;
    u32 offset;
  };
  struct ShaderDef {
    ShaderHandle shaderHandle;
  };
  struct TextureDef {
    std::string name;
    uint32_t binding;
    // format后面加
  };

 public:
  class Builder {
   public:
    Builder& name(const std::string& name) {
      mName = name;
      return *this;
    }
    Builder& shadingModel(ShadingModel model) {
      mShadingModel = model;
      return *this;
    }
    Builder& addParameter(const std::string& name, ParamType paramType, u32 offset) {
      mParamDefs.emplace_back(name, paramType, offset);
      return *this;
    }
    Builder& addShaderModule(ShaderHandle shaderHandle) {
      mShaderDefs.emplace_back(shaderHandle);
      return *this;
    }
    MaterialDefinition build() { return MaterialDefinition(*this); }

   private:
    friend class MaterialDefinition;
    std::string mName;
    ShadingModel mShadingModel = ShadingModel::Lit;
    std::vector<ParamDef> mParamDefs;
    std::vector<ShaderDef> mShaderDefs;
    // std::vector<TextureDef> mTextureDefs;
  };

 private:
  IdType mId{invalidMaterialId};
  std::string mName;
  ShadingModel mShadingModel;
  std::vector<ParamDef> mParamDefs;
  std::vector<ShaderDef> mShaderDefs;
  //   std::vector<TextureDef> mTextureDefs; // 后面加

  MaterialDefinition(const Builder& builder)
      : mName(builder.mName),
        mShadingModel(builder.mShadingModel),
        mParamDefs(builder.mParamDefs),
        mShaderDefs(builder.mShaderDefs) {}
};
}  // namespace pd