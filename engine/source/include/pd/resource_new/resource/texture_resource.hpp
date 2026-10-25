#pragma once

#include "pd/resource_new/resource.hpp"

namespace pd {
class TextureResource : public Resource {
 public:
  struct Info {};
  TextureResource(AssetIdType assetId, ResourceVariantIdType variantId, std::string name, Info info)
      : Resource(assetId, variantId, std::move(name)),
        mInfo(std::move(info)) {}

  ~TextureResource() = default;
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  Info mInfo;
  HwTextureHandle mTexture;
};

template <>
struct TagInfo<TextureResource_t> {
  using ResourceType = TextureResource;
};
}  // namespace pd