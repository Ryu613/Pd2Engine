#include "pd/rendering/resource/texture_resource.hpp"

namespace pd {
namespace {
void copySourceData(std::vector<uint8_t>& target, const void* pSource, size_t size) {
  if (pSource == nullptr) {
    return;
  }
  const auto* pBegin = static_cast<const uint8_t*>(pSource);
  const auto* pEnd = pBegin + size;

  target.assign(pBegin, pEnd);
}
}  // namespace
using Status = TextureResource::Status;

TextureResource::TextureResource(Properties props) noexcept
    : mProperties(std::move(props)) {
  size_t dataSize =
      static_cast<size_t>(mProperties.width) * mProperties.height * mProperties.channel;
  copySourceData(mSourceData, mProperties.pSourceData, dataSize);
}

TextureResource::~TextureResource() {}

TextureResource::Status TextureResource::doLoad() noexcept { return Status::Loaded; }
TextureResource::Status TextureResource::doUnload() noexcept { return Status::Unload; }
}  // namespace pd