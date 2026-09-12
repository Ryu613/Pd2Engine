#pragma once

namespace pd {
enum class AssetType : u8 {
  Gltf,
};

inline u32 invalidAssetId = u32_max;

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