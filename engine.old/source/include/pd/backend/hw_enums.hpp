/**
 * @brief 枚举及结构体
 * @todo 未完整实现
 *
 * @see filament & KDGpu
 */

#pragma once

namespace pd {

enum class TextureType : u8 {
  TextureType1D = 0,
  TextureType2D,
  TextureType3D,
  TextureTypeCube = 3,
};

enum class TextureFormat : u8 {
  Undefined,
  RGBA8Unorm,
  RGBA8SRGB,
};

enum class TextureUsage : u16 {
  None,
  ColorAttachment,
  DepthAttachment,
  StencilAttachment,
  Sampleable,
  Uploadable,
  DefaultUsage = Sampleable | Uploadable,
};

enum class TextureLayout : u16 {
  Undefined = 0,
  General = 1,
};

enum class ColorSpace : u8 {
  SrgbNonLinear,
};

enum class PresentMode : u8 {
  Immediate,
  Mailbox,
  Fifo,
  FifoRelaxed,
};

enum class BufferUsage : u8 {
  VertexBuffer,
  IndexBuffer,
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

enum class ShaderType : u8 {
  None = 0,
  Vertex,
  Fragment,
};

enum class PrimitiveType : u8 {
  TriangleList,
};

struct Extent {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t depth = 0;
};
}  // namespace pd