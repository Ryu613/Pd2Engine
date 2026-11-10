#include "pd/resource/resource.hpp"

namespace pd {
Resource::~Resource() {}

Resource::Resource(u64 sourceId, ResourceVariantIdType variantId, std::string name)
    : mSourceId(sourceId),
      mVariantId(variantId),
      mName(std::move(name)),
      mStatus(ResourceStatus::Unload) {}

Result<void> Resource::load(LoadContext& ctx) noexcept {
  if (mStatus == ResourceStatus::Unload) {
    mStatus = ResourceStatus::Loading;
    if (auto res = doLoad(ctx); !res) {
      mStatus = ResourceStatus::Unload;
      return res;
    }
    mStatus = ResourceStatus::Loaded;
  }
  ctx.loadCount++;
  return {};
}

Result<void> Resource::unload(LoadContext& ctx) noexcept {
  if (mStatus != ResourceStatus::Loaded) {
    return {};
  }
  if (ctx.loadCount > 1) {
    ctx.loadCount--;
    return {};
  }
  if (auto res = doUnload(ctx); !res) {
    return res;
  }
  mStatus = ResourceStatus::Unload;
  ctx.loadCount--;
  return {};
}
}  // namespace pd