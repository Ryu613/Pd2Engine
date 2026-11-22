#include "pd/resource/resource/texture_resource.hpp"

#include "pd/backend/backend.hpp"

namespace pd {

Result<void> TextureResource::doLoad(LoadContext& ctx) noexcept {
  PD_ASSERT(!mInfo.data.empty());
  LOG_DEBUG("loading texture resource: {}", name());
  // create texture if not yet
  if (!mTexture) {
    mTexture = ctx.backend.createTexture({
        .debugName = name(),
        .format = mInfo.format,
        .extent = mInfo.extent,
        .mipLevel = mInfo.mipLevels,
        .arrayLeyers = 1,
        .usage = TextureUsage::ColorAttachment,
        .data = mInfo.data,
    });
    PD_ASSERT(mTexture);
  } else {
    PD_ASSERT_MSG(false, "not supported yet!");
    //   ctx.backend.uploadTexture();
  }
  return {};
}
Result<void> TextureResource::doUnload(LoadContext& ctx) noexcept {
  LOG_DEBUG("unloading texture resource: {}", name());
  if (mTexture) {
    ctx.backend.destroyTexture(mTexture);
  }
  return {};
}
}  // namespace pd