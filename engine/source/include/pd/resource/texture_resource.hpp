#pragma once

#include <span>

#include "pd/resource/resource.hpp"

namespace pd {
class TextureResource : public Resource {
 public:
  struct Desc {};

  ~TextureResource();
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  friend class ResourceManager;

  std::span<uint8_t> mRawData;
  TextureBHandle mTextureHandle;

  explicit TextureResource(
      IdType id, const std::string& name, const Desc info,
      std::span<uint8_t>
          rawData);
};
}  // namespace pd