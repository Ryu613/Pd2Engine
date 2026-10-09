#pragma once

#include "pd/asset/parser/asset_parser.hpp"

#include "fastgltf/types.hpp"
#include "pd/core/utils/map.hpp"

#include "pd/asset/asset.hpp"

namespace pd {
class IFileSystem;
class GltfAsset;
class GltfParser : public IAssetParser {
 public:
  explicit GltfParser(IFileSystem* fs);
  ~GltfParser() override;
  DEFAULT_MOVABLE(GltfParser);
  DELETE_COPY(GltfParser);

  Result<void> parse(Asset& asset) noexcept override;

 private:
  IFileSystem* mFs = nullptr;
  std::filesystem::path mBasePath;

  struct TextureKey {
    u32 imageIndex = invalidAssetId;
    u32 samplerIndex = invalidAssetId;
    bool operator==(const TextureKey&) const = default;
  };
  struct TextureKeyHash {
    size_t operator()(const TextureKey& key) const {
      // high 32 = imageIndex, low 32 = samplerIndex
      const u64 packed = (static_cast<u64>(key.imageIndex) << 32) | key.samplerIndex;
      return static_cast<size_t>(rapidhash(&packed, sizeof(packed)));
    }
  };

  struct CacheData {
    // gltf原texture对应解析后的texture data index
    util::RobinMap<TextureKey, u32, TextureKeyHash> textureIds;
    // material index对应解析后的material data index
    util::RobinMap<u32, u32> materialIds;
  };
  CacheData mCacheData;

  [[deprecated("not used")]]
  void parseMeshes(GltfAsset& asset, const fastgltf::Asset& gltfAsset) noexcept;

  TextureData parseTexture(GltfAsset& asset, const fastgltf::Asset& gltfAsset, size_t imageIndex) noexcept;
  MaterialData parseMaterial(GltfAsset& asset, const fastgltf::Asset& gltfAsset, size_t materialIndex) noexcept;
  Result<void> parseScene(GltfAsset& asset, const fastgltf::Asset& gltfAsset, size_t sceneIndex) noexcept;
};
}  // namespace pd