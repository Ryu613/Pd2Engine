#pragma once

namespace pd {
enum class CmdType : u8 {
  Draw,
  DrawIndex,
  BindPipeline,
  Count,
};

struct DrawCmdArgs {
  static constexpr CmdType type = CmdType::Draw;
  u32 vertexCount = 0;
};
}  // namespace pd