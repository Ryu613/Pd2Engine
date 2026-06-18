#pragma once

#include "pd/backend/hw_enums.hpp"

namespace pd {
struct TextureOptions {
  std::string debugLabel;
  TextureType type = TextureType::TextureType2D;
  TextureFormat format = TextureFormat::RGBA8_Unorm;
  Extent extent = {
      .width = 0,
      .height = 0,
      .depth = 0,
  };
  uint32_t mipLevel = 1;
  uint32_t arrayLeyers = 1;
  TextureUsage usage = TextureUsage::DefaultUsage;
  uint32_t samples = 1;
  TextureLayout initialLayout = TextureLayout::Undefined;
  bool isProtected = false;
};
}  // namespace pd