#pragma once

#include "pd/backend/hw_enums.hpp"
#include "pd/resource/resource.hpp"

namespace pd {
struct TextureResource_t;
/**
 * @brief 纹理资源，在运行时会用做渲染资源
 * @todo only vulkan supported now
 */
class TextureResource : public Resource {
 public:
  struct Properties {
    std::string name;
    std::string path;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channel = 4;
    TextureFormat format = TextureFormat::RGBA8Unorm;
  };

  explicit TextureResource(Properties props, const std::vector<uint8_t>& data) noexcept;
  ~TextureResource();

  TextureResource(const TextureResource&) = delete;
  TextureResource& operator=(const TextureResource&) = delete;
  TextureResource(TextureResource&& rhs) noexcept
      : mProperties(std::move(rhs.mProperties)),
        mSourceData(std::exchange(rhs.mSourceData, {})) {}
  TextureResource& operator=(TextureResource&& rhs) noexcept {
    Resource::operator=(std::move(rhs));
    mProperties = std::exchange(rhs.mProperties, {});
    mSourceData = std::exchange(rhs.mSourceData, {});
    return *this;
  }

  std::string getPath() const noexcept { return mProperties.path; }

 protected:
  void doLoad() noexcept override;
  void doUnload() noexcept override;

 private:
  Properties mProperties;
  std::vector<uint8_t> mSourceData;
};
}  // namespace pd