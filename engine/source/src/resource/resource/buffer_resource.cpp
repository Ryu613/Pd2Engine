#include "pd/resource/resource/buffer_resource.hpp"

#include "pd/backend/backend.hpp"

namespace pd {

Result<void> BufferResource::doLoad(LoadContext& ctx) noexcept {
  LOG_DEBUG("loading buffer resource: {}", name());
  mBuffer = ctx.backend.createBuffer({
      .debugName = name(),
      .usage = mInfo.usage,
      .memoryUsage = MemoryUsage::GpuOnly,
      .deviceSize = mInfo.size,
  });
  PD_ASSERT(mBuffer);
  return {};
}
Result<void> BufferResource::doUnload(LoadContext& ctx) noexcept {
  LOG_DEBUG("unloading buffer resource: {}", name());
  if (mBuffer) {
    ctx.backend.destroyBuffer(mBuffer);
  }
  mBuffer = {};
  return {};
}
}  // namespace pd