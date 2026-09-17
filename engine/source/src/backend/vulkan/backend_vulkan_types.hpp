#pragma once

#include "pd/backend/backend_types.hpp"
#include "vk1_common.hpp"

namespace vk1 {
namespace util {
inline VkShaderStageFlagBits toVkShaderStageFlagBits(pd::ShaderStage stage) {
  switch (stage) {
    using enum pd::ShaderStage;
    case Vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case Fragment:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
      PD_ASSERT_MSG(false, "shader stage enum not supported!");
  }
}
}  // namespace util
}  // namespace vk1