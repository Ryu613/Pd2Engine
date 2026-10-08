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

enum class TextureType : u8 {
  TextureType1D = 0,
  TextureType2D,
  TextureType3D,
  TextureTypeCube = 3,
};

enum class TextureUsage : u8 {
  //   None,
  //   ColorAttachment,
  //   DepthAttachment,
  //   StencilAttachment,
  //   Sampleable,
  //   Uploadable,
  DefaultUsage
};

enum class CullMode : u8 {
  Front,
  Back,
};

enum class FrontFace : u8 {
  Clockwise,
  CounterClockwise,
};

enum class CmdType : u8 {
  BeginRendering,
  EndRendering,
  SetViewport,
  SetScissor,
  BindPipeline,
  BindGeometry,
  UpdateObjectData,
  DrawIndexed,
  ClearColorImage,
  Count,
};

enum class BufferUsage : u8 {
  VertexBuffer,
  IndexBuffer,
  UniformBuffer,
};

enum class SharingMode : u8 {
  Exclusive,
  Concurrent,
};

enum class MemoryUsage : u8 {
  Unknow,
  GpuOnly,
  CpuOnly,
  CpuToGpu,
  GpuToCpu,
  CpuCopy,
  GpuLazilyAllocated,
};
}  // namespace pd