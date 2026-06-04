#pragma once

#include "pd/asset/asset_parser.hpp"

#include "fastgltf/types.hpp"

namespace pd {
class FileSystem;
class EntityManager;
class ResourceManager;
class GltfParser : public AssetParser {
 public:
  GltfParser(FileSystem& fs, EntityManager& em, ResourceManager& rm) noexcept;
  ~GltfParser() = default;
  MOVABLE_ONLY(GltfParser);

  std::expected<void, AssetError> parse(Asset& asset) noexcept override;

 private:
  FileSystem& mFileSystem;
  EntityManager& mEntityManager;
  ResourceManager& mResourceManager;
  std::filesystem::path mBasePath;
  std::vector<TextureFormat> mTextureFormatCache;

  void parseMeshes(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept;
  void parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                  size_t gltfSceneIndex) noexcept;
};
}  // namespace pd