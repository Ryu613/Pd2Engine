#pragma once

#include "pd/asset/parser/asset_parser.hpp"

#include "fastgltf/types.hpp"
#include "pd/core/utils/map.hpp"

#include "pd/asset/asset.hpp"
#include "pd/asset/compiler/texture_compiler.hpp"

namespace pd {
class IFileSystem;
class GltfAsset;
class GltfParser : public IAssetParser {
 public:
  explicit GltfParser(IFileSystem& fs);
  ~GltfParser() override;
  DELETE_COPY_MOVE(GltfParser);

  Result<void> parse(Asset& asset) noexcept override;

 private:
  IFileSystem& mFs;

  std::filesystem::path mBasePath;

  TextureCompiler mTextureCompiler;

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
    // gltf原texture对应解析后的texture data的index
    util::RobinMap<TextureKey, u32, TextureKeyHash> textureIds;
    // material index对应解析后的material data index
    util::RobinMap<u32, u32> materialIds;
  };
  CacheData mCacheData;

  void parseTexture(GltfAsset& asset, const fastgltf::Asset& gltfAsset, u32& materialTextureIndex, u32 textureIndex,
                    u32 samplerIndex) noexcept;
  void parseMaterial(GltfAsset& asset, const fastgltf::Asset& gltfAsset, GltfAsset::MeshData::Primitive& primitive,
                     u32 materialIndex) noexcept;
  Result<void> parseScene(GltfAsset& asset, const fastgltf::Asset& gltfAsset, size_t sceneIndex) noexcept;

  GltfAsset::SceneNode makeParsedNode(const u32 nodeIndex, const math::mat4& transform);
};
}  // namespace pd