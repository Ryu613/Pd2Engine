#pragma once

namespace pd {
class TextureData;
class ITextureDecoder {
 public:
  ITextureDecoder() = default;
  virtual ~ITextureDecoder() = default;
  DELETE_COPY(ITextureDecoder);
  DEFAULT_MOVABLE(ITextureDecoder);

  virtual Result<std::vector<const std::byte>> decode(TextureData& texData) noexcept = 0;
};
}  // namespace pd