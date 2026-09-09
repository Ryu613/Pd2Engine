#pragma once

namespace pd {
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
  u32 frameIndex = u32_max;
  u32 imageIndex = u32_max;
  std::array<u64, 10> args;
};
struct FrameData {
  u32 frameIndex = u32_max;
  u32 swapchainImageIndex = u32_max;
};
}  // namespace pd