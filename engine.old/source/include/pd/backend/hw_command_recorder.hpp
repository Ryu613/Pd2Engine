#pragma once
#include "pd/backend/hw_enums.hpp"
#include "pd/backend/hw_resource.hpp"
#include "pd/backend/hw_handle.hpp"

namespace pd {
struct CommandRecorder_t;
struct Pipeline_t;
struct Buffer_t;
struct GraphicsState {
  HwHandle<Pipeline_t> graphicsPipeline{};
  HwHandle<Buffer_t> vertexBuffer{};
  u32 vertexOffset = 0;
  HwHandle<Buffer_t> indexBuffer{};
  u32 indexOffset = 0;
};

struct DrawIndexedCommand {
  u32 indexCount = 0;
  u32 instanceCount = 1;
  u32 firstIndex = 0;
  i32 vertexOffset = 0;
  u32 firstInstance = 0;
};

struct HwCommandRecorder : public HwResource {};
}  // namespace pd