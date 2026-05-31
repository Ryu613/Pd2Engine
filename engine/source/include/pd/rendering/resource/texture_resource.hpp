#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
enum class TextureFormat : uint16_t {
  RGBA8_UNORM,
  RGBA8_SRGB,
};
/**
 * @brief 纹理资源，在运行时会用做渲染资源
 * @todo only vulkan supported now
 */
class TextureResource : public Resource {
 public:
  struct Properties {
    std::string name;
    std::string path;
    uint32_t width;
    uint32_t height;
    uint32_t channel = 4;
    TextureFormat format = TextureFormat::RGBA8_UNORM;
    void* pSourceData = nullptr;
  };

  explicit TextureResource(Properties props) noexcept;
  ~TextureResource();
  MOVABLE_ONLY(TextureResource);

 protected:
  Status doLoad() noexcept override;
  Status doUnload() noexcept override;

 private:
  Properties mProperties;
  std::vector<uint8_t> mSourceData;
};
}  // namespace pd