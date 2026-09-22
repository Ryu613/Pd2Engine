#pragma once

#include "backend_enums.hpp"
#include "pd/core/math/math.hpp"
#include "pd/core/utils/handle.hpp"

namespace pd {

template <typename T>
using HwHandle = TypedHandle<T>;

// handle types
// 类型别名，方便外部引用，与实际类型解耦
struct Buffer_t;
struct Texture_t;
struct PipelineLayout_t;
struct GraphicsPipeline_t;
struct ShaderModule_t;

using HwBufferHandle = HwHandle<Buffer_t>;
using HwTextureHandle = HwHandle<Texture_t>;
using HwPipelineLayoutHandle = HwHandle<PipelineLayout_t>;
using HwGraphicsPipelineHandle = HwHandle<GraphicsPipeline_t>;
using HwShaderModuleHandle = HwHandle<ShaderModule_t>;

// commands
struct BeginRenderingArgs {
  static constexpr CmdType type = CmdType::BeginRendering;
};

struct EndRenderingArgs {
  static constexpr CmdType type = CmdType::EndRendering;
};

struct SetViewportArgs {
  static constexpr CmdType type = CmdType::SetViewport;
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
  float minDepth = 0.0f;
  float maxDepth = 1.0f;
};

struct SetScissorArgs {
  static constexpr CmdType type = CmdType::SetScissor;
  float offsetX = 0.0f;
  float offsetY = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

struct BindPipelineArgs {
  static constexpr CmdType type = CmdType::BindPipeline;
  HwBufferHandle vertexBuffer;
  u32 vertexBufferOffset = 0;
  HwBufferHandle indexBuffer;
  u32 indexBufferOffset = 0;
  HwGraphicsPipelineHandle pipeline;
};

struct DrawIndexedArgs {
  static constexpr CmdType type = CmdType::DrawIndexed;
  u32 indexCount = 0;
  u32 instanceCount = 1;
  u32 firstIndex = 0;
  u32 vertexOffset = 0;
  u32 firstInstance = 0;
};

struct ClearColorImageArgs {
  static constexpr CmdType type = CmdType::ClearColorImage;
};

// common types
struct FrameData {
  u32 frameIndex = u32_max;
  u32 swapchainImageIndex = u32_max;
};

struct ShaderProgram {
  size_t shaderCodeIndex;
  ShaderStage stage;
  std::string entryPoint = "main";
};

struct ShaderCode {
  ShaderLang lang = ShaderLang::Slang;
  std::vector<u8> code;
  // todo: support shader import
  //   std::vector<size_t> dependencies;  // other shader code index
};

struct VertexDesc {
  struct BufferLayout {
    u32 binding;
    u32 stride;
    VertexRate vertexRate = VertexRate::Vertex;
  };
  struct Attribute {
    u32 location;
    u32 binding;
    TextureFormat format;
  };
  BufferLayout layout;
  Attribute attribute;
};

struct PipelineLayoutDesc {
  std::string_view debugName;
};
struct ShaderData {
  std::vector<u8> spirvCode;
  // todo: reflection info
};

struct ShaderDesc {
  std::string moduleName;
  std::string modulePath;
  std::vector<u8> code;
  ShaderLang lang = ShaderLang::Slang;
};

struct BufferCreateDesc {
  std::string_view debugName;
  BufferUsage usage = BufferUsage::VertexBuffer;
  SharingMode sharingMode = SharingMode::Exclusive;
  MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
  uint64_t deviceSize = 0;
};

struct BufferWriteDesc {
  HwBufferHandle buffer;
  const void* pData = nullptr;
  u64 deviceSize = 0;
  u32 offset = 0;
};

struct GraphicsPipelineDesc {
  std::string_view debugName;
  HwPipelineLayoutHandle layout;
  std::vector<ShaderData> shaderDatas;
  std::vector<ShaderProgram> shaderPrograms;
};

struct PipelineData {
  HwPipelineLayoutHandle layout;
  HwGraphicsPipelineHandle pipeline;
};

struct Vertex {
  math::vec3 pos;
  math::vec3 normal;
  math::vec2 uv;
};

struct UniformBufferObject {
  math::mat4 model{1.0f};
  math::mat4 view{1.0f};
  math::mat4 proj{1.0f};
};
}  // namespace pd