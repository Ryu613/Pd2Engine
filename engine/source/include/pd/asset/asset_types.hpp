#pragma once

namespace pd {
enum class AssetType : u8 {
  Gltf,
};

inline u32 invalidAssetId = u32_max;

using AssetIdType = u32;

struct DataInfo {
  AssetIdType dataId = invalidAssetId;
  std::string name;
};
}  // namespace pd