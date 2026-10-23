#include "pd/asset/compiler/decoder/stb_decoder.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace pd {
Result<ImageData> StbDecoder::decode(TextureData& texData) noexcept {
  ImageData result;
  std::visit(
      [&result, &texData](const auto& arg) {
        using SourceType = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<SourceType, TextureData::DataUri>) {
        } else if constexpr (std::is_same_v<SourceType, TextureData::ExternalFile>) {
          auto& sourceInfo = std::get<TextureData::ExternalFile>(texData.source);
          int width = 0;
          int height = 0;
          int channels = 0;

          stbi_uc* texels = stbi_load(sourceInfo.uri.c_str(),  //
                                      &width,                  //
                                      &height,                 //
                                      &channels,               //
                                      STBI_rgb_alpha);
          if (!texels) {
            PD_ASSERT_MSG(false, "failed to load image!");
          }
          texData.width = static_cast<u32>(width);
          texData.height = static_cast<u32>(height);
          texData.channels = static_cast<u32>(channels);

          const std::size_t byteCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;

          std::vector<std::byte> pixels(byteCount);
          std::memcpy(pixels.data(), texels, byteCount);

          result.bytes = std::move(pixels);
          stbi_image_free(texels);
        } else if constexpr (std::is_same_v<SourceType, TextureData::BufferView>) {
          auto& sourceInfo = std::get<TextureData::BufferView>(texData.source);
          int width = 0;
          int height = 0;
          int channels = 0;
          stbi_uc* texels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(sourceInfo.dataPtr),  //
                                                  sourceInfo.byteLength,                                 //
                                                  &width,                                                //
                                                  &height,                                               //
                                                  &channels,                                             //
                                                  STBI_rgb_alpha);
          if (!texels) {
            PD_ASSERT_MSG(false, "failed to load image!");
          }
          texData.width = static_cast<u32>(width);
          texData.height = static_cast<u32>(height);
          texData.channels = static_cast<u32>(channels);

          const std::size_t byteCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;

          std::vector<std::byte> pixels(byteCount);
          std::memcpy(pixels.data(), texels, byteCount);

          result.bytes = std::move(pixels);
          stbi_image_free(texels);
        } else {
          PD_ASSERT_MSG(false, "not supported texture source type");
        }
      },
      texData.source);

  return result;
}
}  // namespace pd