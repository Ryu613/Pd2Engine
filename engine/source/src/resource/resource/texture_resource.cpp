#include "pd/resource/resource/texture_resource.hpp"

namespace pd {

using Status = TextureResource::Status;

TextureResource::TextureResource(IdType id, Properties props) noexcept
    : Resource(id),
      mProperties(std::move(props)) {}

void TextureResource::doLoad(IBackend& backend) noexcept {}
void TextureResource::doUnload(IBackend& backend) noexcept {}
}  // namespace pd