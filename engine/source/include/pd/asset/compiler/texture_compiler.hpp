#pragma once

#include "pd/core/utils/map.hpp"

#include "pd/asset/asset_types.hpp"
#include "pd/asset/compiler/decoder/texture_decoder.hpp"

namespace pd {
class TextureData;
class TextureCompiler {
 public:
  TextureCompiler();
  ~TextureCompiler();
  DELETE_COPY(TextureCompiler);
  DEFAULT_MOVABLE(TextureCompiler);

  Result<void> compile(TextureData& tex) noexcept;

 private:
  util::RobinMap<TextureDecoderType, std::unique_ptr<ITextureDecoder>> mDecoders;

  void initDecoders() noexcept;
};
}  // namespace pd