#pragma once

namespace pd {
enum class AssetType : u8 {
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

inline constexpr u32 invalidAssetId = u32_max;

using AssetIdType = u32;
using AssetPathType = std::string;

struct AssetHandle {
  AssetIdType id = invalidAssetId;
};

struct DataInfo {
  AssetIdType dataId;
  std::string name;
};
}  // namespace pd