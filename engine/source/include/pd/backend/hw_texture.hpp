#pragma once

#include "pd/backend/texture_options.hpp"

namespace pd {
/**
 * @brief gpu层texture表示,纯结构体，只用于记录跨backend的基本信息
 *
 */
struct HwTexture {
  TextureOptions options;
};
}  // namespace pd