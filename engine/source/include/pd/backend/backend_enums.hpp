#pragma once

namespace pd {
enum class ShaderLang : u8 {
  Slang,
};
enum class ShaderStage : u8 {
  Vertex,
  Fragment,
};

enum class VertexRate : u8 {
  Vertex,
  Instance,
};

enum class TextureFormat : u8 {
  Undefined,
  RGBA8Unorm,
  RGBA8SRGB,
};

enum class CmdType : u8 {
  BeginRendering,
  EndRendering,
  SetViewport,
  SetScissor,
  BindPipeline,
  ClearColorImage,
  Count,
};
}  // namespace pd