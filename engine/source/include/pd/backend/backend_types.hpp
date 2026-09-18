#pragma once

#include "backend_enums.hpp"

namespace pd {

/**
 * @brief RHI句柄
 *
 * 设计思路：纯数据，保持简单，内部使用，不过度封装访问性
 */
using HandleIdType = u32;
struct HandleData {
  static constexpr u32 invalidId = u32_max;
  HandleIdType id = invalidId;
  u32 gen = 0;
};
template <typename T>
struct HwHandle {
  HandleData data;

  friend bool operator==(HwHandle lhs, HwHandle rhs) noexcept {
    if (lhs.data.id == HandleData::invalidId && rhs.data.id == HandleData::invalidId) {
      return true;
    }
    return lhs.data.id == rhs.data.id && lhs.data.gen == rhs.data.gen;
  }

  friend bool operator!=(HwHandle a, HwHandle b) noexcept { return !(a == b); }

  // 只表示存在，不表示有效
  explicit operator bool() const noexcept { return data.id != HandleData::invalidId; }
};

// handle types
// 类型别名，方便外部引用，与实际类型解耦
struct Buffer_t;
struct Texture_t;
struct PipelineLayout_t;
struct GraphicsPipeline_t;

using HwBufferHandle = HwHandle<Buffer_t>;
using HwTextureHandle = HwHandle<Texture_t>;
using HwPipelineLayoutHandle = HwHandle<PipelineLayout_t>;
using HwGraphicsPipelineHandle = HwHandle<GraphicsPipeline_t>;

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
  HwGraphicsPipelineHandle pipeline;
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

struct ShaderSpec {
  std::string moduleName;
  std::string modulePath;
  std::vector<u8> code;
  ShaderLang lang = ShaderLang::Slang;
};
struct GraphicsPipelineDesc {
  std::string_view debugName;
  std::vector<ShaderData> shaderDatas;
  std::vector<ShaderProgram> shaderPrograms;
};

struct PipelineData {
  HwPipelineLayoutHandle layout;
  HwGraphicsPipelineHandle pipeline;
};
}  // namespace pd