#include "pd/asset/compiler/texture_compiler.hpp"

#include "pd/asset/compiler/decoder/stb_decoder.hpp"

namespace pd {
TextureCompiler::TextureCompiler() { initDecoders(); }
TextureCompiler::~TextureCompiler() {}

Result<ImageData> TextureCompiler::compile(TextureData& tex) noexcept {
  // 当前只支持stb
  PD_ASSERT(tex.decoderType == TextureDecoderType::Stb);
  return mDecoders[tex.decoderType]->decode(tex);
}

void TextureCompiler::initDecoders() noexcept {
  mDecoders.emplace(TextureDecoderType::Stb, std::make_unique<StbDecoder>());
}
}  // namespace pd