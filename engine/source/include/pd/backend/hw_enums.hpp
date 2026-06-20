/**
 * @brief 枚举及结构体
 * @todo 未完整实现
 *
 * @see filament & KDGpu
 */

#pragma once

namespace pd {

enum class TextureType : uint8_t {
  TextureType1D = 0,
  TextureType2D,
  TextureType3D,
  TextureTypeCube = 3,
};

enum class TextureFormat : uint16_t {
  Undefined,
  RGBA8Unorm,
  RGBA8SRGB,
};

enum class TextureUsage : uint16_t {
  None,
  ColorAttachment,
  DepthAttachment,
  StencilAttachment,
  Sampleable,
  Uploadable,
  DefaultUsage = Sampleable | Uploadable,
};

enum class TextureLayout : uint16_t {
  Undefined = 0,
  General = 1,
};

enum class ColorSpace : uint8_t {
  SrgbNonLinear,
};

enum class PresentMode : uint8_t {
  Immediate,
  Mailbox,
  Fifo,
  FifoRelaxed,
};

struct Extent {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t depth = 0;
};
}  // namespace pd