#pragma once

#include "pd/backend/backend_types.hpp"

namespace pd {
enum class ParseType : u8 {
  Gltf,
  Shader,
};

enum class MaterialType : u8 {
  PbrMetallicRoughness,
};

enum class TextureDecoderType : u8 {
  Stb,
  Ktx2,
};

using AssetIdType = u32;
inline constexpr AssetIdType invalidAssetId = std::numeric_limits<AssetIdType>::max();

using AssetPathType = std::string;
using VertexData = pd::Vertex;

struct AssetHandle {
  AssetIdType id = invalidAssetId;
};

struct ImageData {
  std::vector<std::byte> bytes;
};
struct SamplerData {
  // todo
};
struct TextureData {
  std::string name;
  std::string uri;
  const std::byte* dataPtr = nullptr;
  size_t byteLength = 0;
  TextureDecoderType decoderType = TextureDecoderType::Stb;
  u32 width = 0;
  u32 height = 0;
  u32 mipLevel = 0;
  u32 imageIndex = invalidAssetId;
  u32 samplerIndex = invalidAssetId;
};

}  // namespace pd