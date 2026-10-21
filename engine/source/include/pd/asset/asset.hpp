#pragma once

#include "pd/asset/asset_types.hpp"
#include "pd/backend/backend_types.hpp"
#include "pd/core/math/math.hpp"

#include <span>

namespace pd {

// class TextureData {
//  public:
//   struct Props {
//     std::string mPath;
//     u32 width = 0;
//     u32 height = 0;
//     // format
//   };
//   TextureData(const DataInfo& info, const Props& props, std::vector<uint8_t>&& pixels)
//       : mDataInfo(info),
//         mProps(props),
//         mPixels(std::move(pixels)) {}

//   DEFAULT_MOVABLE(TextureData);
//   DELETE_COPY(TextureData);

//   const DataInfo& info() const noexcept { return mDataInfo; }
//   const Props& props() const noexcept { return mProps; }
//   std::span<const uint8_t> getPixels() const noexcept { return mPixels; }

//  private:
//   DataInfo mDataInfo;
//   Props mProps;
//   std::vector<uint8_t> mPixels;
// };

using VertexData = pd::Vertex;

struct MeshData {
  struct SubMesh {
    std::string name;
    std::vector<VertexData> vertices;
    std::vector<u32> indices;
    u32 materialIndex = invalidAssetId;
  };

  DataInfo dataInfo;
  std::vector<SubMesh> subMeshes;
};

struct SceneNode {
  std::string name;
  math::vec3 pos{};
  math::vec3 eulerAngles{};
  math::vec3 scale{1.f};
  u32 meshId = invalidAssetId;
};

struct ShaderInfo {
  std::string moduleName;
  std::string modulePath;
};

class Asset {
 public:
  struct MetaInfo {
    std::string name;
    std::string path;
    AssetType assetType = AssetType::Gltf;
    std::optional<ShaderInfo> shaderInfo;
    // std::optional<TextureInfo> shaderInfo;
  };

  ~Asset() = default;
  DEFAULT_MOVABLE(Asset);
  DELETE_COPY(Asset);

  AssetIdType id() const noexcept { return mId; }
  const MetaInfo& info() const noexcept { return mInfo; }

 protected:
  explicit Asset(AssetIdType id, const MetaInfo& info)
      : mId(std::move(id)),
        mInfo(info) {}

 private:
  friend class AssetManager;

  AssetIdType mId;
  MetaInfo mInfo;
};

class MeshAsset : public Asset {
 public:
  ~MeshAsset() = default;
  DEFAULT_MOVABLE(MeshAsset);
  DELETE_COPY(MeshAsset);

 private:
  friend class AssetManager;
  friend class GltfParser;

  std::vector<VertexData> vertices;
  std::vector<u32> indices;
  u32 materialIndex = invalidAssetId;

  explicit MeshAsset(AssetIdType id, MetaInfo info)
      : Asset(id, info) {}
};

class TextureAsset : public Asset {
 public:
  ~TextureAsset() = default;
  DEFAULT_MOVABLE(TextureAsset);
  DELETE_COPY(TextureAsset);

 private:
  friend class AssetManager;
  friend class GltfParser;

  struct Info {
    u32 width = 0;
    u32 height = 0;
    u32 mipsLevel = 0;
  };

  Info mInfo;
  std::string uri;

  explicit TextureAsset(AssetIdType id, MetaInfo info)
      : Asset(id, info) {}
};

class GltfAsset : public Asset {
 private:
  friend class AssetManager;
  friend class GltfParser;
  struct ImageData {
    std::vector<std::byte> ktx2Data;
  };
  struct SamplerData {
    // todo
  };
  struct TextureData {
    std::string name;
    u32 imageIndex = invalidAssetId;
    //   u32 samplerIndex = invalidAssetId;
  };
  struct MeshData {
    struct Primitive {
      std::string name;
      std::vector<VertexData> mVertices;
      std::vector<u32> mIndices;
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
  DEFAULT_MOVABLE(GltfAsset);
  DELETE_COPY(GltfAsset);

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
      : Asset(id, info) {}
};

class ShaderAsset : public Asset {
 public:
  ~ShaderAsset() = default;
  DEFAULT_MOVABLE(ShaderAsset);
  DELETE_COPY(ShaderAsset);

  std::span<const std::byte> sources() const noexcept { return mParsedCode; }

 private:
  friend class AssetManager;
  friend class ShaderParser;

  struct ReflectionInfo {
    // todo
  };
  std::vector<std::byte> mParsedCode;
  ReflectionInfo mReflectionInfo;

  explicit ShaderAsset(AssetIdType id, const MetaInfo& info)
      : Asset(id, info) {}

  void setParsedCode(std::span<u8> code) noexcept {
    mParsedCode.clear();
    mParsedCode.reserve(code.size());
    std::transform(code.begin(), code.end(), std::back_inserter(mParsedCode),
                   [](u8 b) { return static_cast<std::byte>(b); });
  }
};
}  // namespace pd