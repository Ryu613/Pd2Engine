#pragma once

#include "pd/core/utils/handle.hpp"

namespace pd {

template <typename T>
using BackendHandle = TypedHandle<T>;

enum class CmdType : u8 {
  BeginRendering,
  EndRendering,
  SetViewport,
  SetScissor,
  BindPipeline,
  ClearColorImage,
  Count,
};

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
};

struct ClearColorImageArgs {
  static constexpr CmdType type = CmdType::ClearColorImage;
};

struct CommandPayload {
  CmdType type;
  std::array<u64, 10> args;
};
struct FrameData {
  u32 frameIndex = u32_max;
  u32 swapchainImageIndex = u32_max;
};

struct Pipeline_t;
}  // namespace pd