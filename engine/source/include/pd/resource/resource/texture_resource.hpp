#pragma once

#include "pd/backend/hw_enums.hpp"
#include "pd/resource/resource.hpp"
#include "pd/resource/resource_types.hpp"
#include "pd/backend/hw_texture.hpp"
#include "pd/backend/hw_handle.hpp"

namespace pd {
struct TextureResource_t;
/**
 * @brief 纹理资源，在运行时会用做渲染资源
 * @todo only vulkan supported now
 */
class TextureResource : public Resource {
 public:
  struct Properties {
    TextureInfo info;
    TextureFormat format = TextureFormat::RGBA8Unorm;
    TextureUsage usage = TextureUsage::None;
    // SamplerType sampler = SamplerType::2D;
  };

  explicit TextureResource(IdType id, Properties props) noexcept;
  ~TextureResource() = default;
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

  //   TextureResource(TextureResource&& rhs) noexcept
  //       : mProperties(std::move(rhs.mProperties)),
  //         mSourceData(std::exchange(rhs.mSourceData, {})) {}
  //   TextureResource& operator=(TextureResource&& rhs) noexcept {
  //     if (this != &rhs) {
  //       Resource::operator=(std::move(rhs));
  //       mProperties = std::exchange(rhs.mProperties, {});
  //       mSourceData = std::exchange(rhs.mSourceData, {});
  //     }
  //     return *this;
  //   }

  //   std::string getPath() const noexcept { return mProperties.path; }

 protected:
  void doLoad(IBackend& backend) noexcept override;
  void doUnload(IBackend& backend) noexcept override;

 private:
  Properties mProperties;
  HwHandle<Texture_t> mTexture;
  //   HwHandle<Sampler_t> mSampler;
  std::vector<uint8_t> mRawData;
};
}  // namespace pd