#pragma once

#include "pd/asset/asset_parser.hpp"

#include "fastgltf/types.hpp"

namespace pd {
class IFileSystem;
class ResourceManager;
class GltfParser : public IAssetParser {
 public:
  GltfParser() noexcept = default;
  GltfParser(IFileSystem* fs, ResourceManager* rm) noexcept;
  ~GltfParser() override = default;
  MOVABLE_ONLY(GltfParser);

  Asset::AssetResult<void> parse(Asset& asset) noexcept override;

 private:
  IFileSystem* mFileSystem = nullptr;
  ResourceManager* mResourceManager = nullptr;
  std::filesystem::path mBasePath;
  std::vector<TextureFormat> mTextureFormatCache;

  void parseMeshes(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                  size_t gltfSceneIndex) noexcept;
};
}  // namespace pd