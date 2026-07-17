#pragma once

#include "pd/backend/hw_enums.hpp"
#include "pd/backend/hw_resource.hpp"

namespace pd {
struct HwShaderProgram : HwResource {
  ShaderType shaderType = ShaderType::None;

  std::vector<uint8_t> spirvCode;
  std::string_view entryName = "main";
};

struct VertexAttributeDesc {
  TextureFormat format = TextureFormat::Undefined;
  uint32_t arraySize = 1;
  uint32_t bufferIndex = 0;
  uint32_t offset = 0;
  uint32_t elementStride = 0;
};

struct VertexInputOptions {
  std::vector<VertexAttributeDesc> vertexBufferLayouts;
};

struct HwGraphicsPipeline : HwResource {
  PrimitiveType primType = PrimitiveType::TriangleList;
  VertexInputOptions vertexOptions;

  // shaders
  HwShaderProgram vertexShader;    // vertex shader
  HwShaderProgram fragmentShader;  // pixel shader/ fragment shader

  // todo(ryu613): binding layouts
};
}  // namespace pd