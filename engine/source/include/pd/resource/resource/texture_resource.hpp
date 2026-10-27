#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class TextureResource : public Resource {
 public:
  struct Info {};

  ~TextureResource() = default;
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  friend class ResourceManager;

  Info mInfo;
  HwTextureHandle mTexture;

  explicit TextureResource(AssetIdType assetId, ResourceVariantIdType variantId = defaultVariantId,
                           std::string name = "texture")
      : Resource(assetId, variantId, std::move(name)) {}
};

template <>
struct TagInfo<TextureResource_t> {
  using ResourceType = TextureResource;
};
}  // namespace pd