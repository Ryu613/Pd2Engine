#pragma once

#include "pd/asset/asset_parser.hpp"

#include "fastgltf/types.hpp"

namespace pd {
class GltfParser : public AssetParser {
 public:
  GltfParser() noexcept = default;
  ~GltfParser() = default;
  MOVABLE_ONLY(GltfParser);

  std::unique_ptr<Asset> parse(FileSystem& fs,
                               const Asset::Info& assetInfo) noexcept override;

 private:
  std::filesystem::path mBasePath;
  std::vector<TextureFormat> mTextureFormatCache;

  void parseMeshes(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                  size_t gltfSceneIndex) noexcept;
};
}  // namespace pd