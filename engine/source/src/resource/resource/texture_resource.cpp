#include "pd/resource/resource/texture_resource.hpp"

namespace pd {

using Status = TextureResource::Status;

TextureResource::TextureResource(Properties props,
                                 const std::vector<uint8_t>& data) noexcept
    : Resource(props.path),
      mProperties(std::move(props)),
      mSourceData(data) {}

TextureResource::~TextureResource() {}

void TextureResource::doLoad(IBackend& backend) noexcept {}
void TextureResource::doUnload(IBackend& backend) noexcept {}
}  // namespace pd