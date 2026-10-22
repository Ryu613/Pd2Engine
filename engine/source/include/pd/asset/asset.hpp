#pragma once

#include "pd/core/utils/handle.hpp"
#include "pd/core/utils/traits.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/core/math/math.hpp"

#include <span>

namespace pd {

class Asset {
 public:
  static constexpr AssetIdType invalidId = invalidAssetId;
  struct ShaderInfo {
    std::string moduleName;
    std::string modulePath;
  };
  struct MetaInfo {
    std::string name;
    std::string path;
    ParseType parseType = ParseType::Gltf;
    std::optional<ShaderInfo> shaderInfo;
    // std::optional<TextureInfo> shaderInfo;
  };

  virtual ~Asset() = default;
  DELETE_COPY_MOVE(Asset);

  AssetIdType id() const noexcept { return mId; }
  const MetaInfo& info() const noexcept { return mInfo; }

 protected:
  explicit Asset(AssetIdType id, MetaInfo info)
      : mId(id),
        mInfo(std::move(info)) {}

 private:
  const AssetIdType mId;
  MetaInfo mInfo;
};

// class MeshAsset : public Asset {
//  public:
//   ~MeshAsset() = default;
//   DEFAULT_MOVABLE(MeshAsset);
//   DELETE_COPY(MeshAsset);

//  private:
//   friend class AssetManager;
//   friend class GltfParser;

//   std::vector<VertexData> vertices;
//   std::vector<u32> indices;
//   u32 materialIndex = invalidAssetId;

//   explicit MeshAsset(AssetIdType id, MetaInfo info)
//       : Asset(id, info) {}
// };

// class TextureAsset : public Asset {
//  public:
//   ~TextureAsset() = default;
//   DEFAULT_MOVABLE(TextureAsset);
//   DELETE_COPY(TextureAsset);

//  private:
//   friend class AssetManager;
//   friend class GltfParser;

//   struct Info {
//     u32 width = 0;
//     u32 height = 0;
//     u32 mipsLevel = 0;
//   };

//   Info mInfo;
//   std::string uri;

//   explicit TextureAsset(AssetIdType id, MetaInfo info)
//       : Asset(id, info) {}
// };

class GltfAsset : public Asset {
 private:
  friend class AssetManager;
  friend class GltfParser;
  struct SceneNode {
    std::string name;
    math::vec3 pos{};
    math::vec3 eulerAngles{};
    math::vec3 scale{1.f};
    u32 meshId = invalidAssetId;
  };
  struct MeshData {
    struct Primitive {
      std::string name;
      std::vector<VertexData> vertices;
      std::vector<u32> indices;
      u32 materialIndex = invalidAssetId;
    };
    std::string name;
    std::vector<Primitive> primitives;
  };
  struct GltfPbrMaterialData {
    static constexpr MaterialType materialType = MaterialType::PbrMetallicRoughness;
    std::string name;
    math::vec3 baseColorFactor;
    u32 baseColorTextureIndex = invalidAssetId;
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.0f;
    u32 metallicRoughnessTextureIndex = invalidAssetId;
    u32 normalTextureIndex = invalidAssetId;
    math::vec3 emissiveFactor;
  };

 public:
  ~GltfAsset() = default;
  DELETE_COPY_MOVE(GltfAsset);

  std::span<const SceneNode> nodes() const noexcept { return mNodes; }
  std::span<const MeshData> meshes() const noexcept { return mMeshes; }
  std::span<const GltfPbrMaterialData> materials() const noexcept { return mMaterials; }
  std::span<const TextureData> textures() const noexcept { return mTextures; }

 private:
  std::vector<SceneNode> mNodes;
  std::vector<MeshData> mMeshes;
  std::vector<GltfPbrMaterialData> mMaterials;
  std::vector<TextureData> mTextures;
  std::vector<ImageData> mImages;

  explicit GltfAsset(AssetIdType id, MetaInfo info)
      : Asset(id, std::move(info)) {}
};

class ShaderAsset : public Asset {
 public:
  ~ShaderAsset() = default;
  DELETE_COPY_MOVE(ShaderAsset);

  std::span<const std::byte> sources() const noexcept { return mParsedCode; }

 private:
  friend class AssetManager;
  friend class ShaderParser;

  struct ReflectionInfo {
    // todo
  };
  std::vector<std::byte> mParsedCode;
  ReflectionInfo mReflectionInfo;

  explicit ShaderAsset(AssetIdType id, MetaInfo info)
      : Asset(id, std::move(info)) {}

  void setParsedCode(std::span<u8> code) noexcept {
    mParsedCode.clear();
    mParsedCode.reserve(code.size());
    std::transform(code.begin(), code.end(), std::back_inserter(mParsedCode),
                   [](u8 b) { return static_cast<std::byte>(b); });
  }
};
}  // namespace pd