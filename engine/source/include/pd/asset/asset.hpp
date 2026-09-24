#pragma once

#include "pd/asset/asset_types.hpp"
#include "pd/core/math/math.hpp"

#include <span>

namespace pd {

class TextureData {
 public:
  struct Props {
    std::string mPath;
    u32 width = 0;
    u32 height = 0;
    // format
  };
  TextureData(const DataInfo& info, const Props& props, std::vector<uint8_t>&& pixels)
      : mDataInfo(info),
        mProps(props),
        mPixels(std::move(pixels)) {}

  DEFAULT_MOVABLE(TextureData);
  DELETE_COPY(TextureData);

  const DataInfo& info() const noexcept { return mDataInfo; }
  const Props& props() const noexcept { return mProps; }
  std::span<const uint8_t> getPixels() const noexcept { return mPixels; }

 private:
  DataInfo mDataInfo;
  Props mProps;
  std::vector<uint8_t> mPixels;
};

struct VertexData {
  math::vec3 position{};
  math::vec2 uv{};
  math::vec3 normal{};
  math::vec4 tangent{1.0f, 0.0f, 0.0f, 1.0f};
};

struct MeshData {
  struct SubMesh {
    std::string name;
    std::vector<VertexData> vertices;
    std::vector<u32> indices;
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
  // todo: texture ids
  // u32 parentIndex = invalidAssetId;
  // u32 firstChild = invalidAssetId;
  // u32 nextSibling = invalidAssetId;
};

struct ShaderInfo {
  std::string moduleName;
  std::string modulePath;
};

class Asset {
 public:
  struct CreateInfo {
    std::string name;
    std::string path;
    AssetType parseType = AssetType::Gltf;
    ShaderInfo shaderInfo;
  };

  ~Asset() = default;
  DEFAULT_MOVABLE(Asset);
  DELETE_COPY(Asset);

  AssetIdType id() const noexcept { return mId; }
  const CreateInfo& info() const noexcept { return mInfo; }

 protected:
  explicit Asset(AssetIdType id, const CreateInfo& info)
      : mId(std::move(id)),
        mInfo(info) {}

 private:
  friend class AssetManager;

  AssetIdType mId;
  CreateInfo mInfo;
};

class GltfAsset : public Asset {
 public:
  ~GltfAsset() = default;
  DEFAULT_MOVABLE(GltfAsset);
  DELETE_COPY(GltfAsset);
  std::span<const TextureData> textures() const noexcept { return mTextures; }
  std::span<const MeshData> meshes() const noexcept { return mMeshes; }
  std::span<const SceneNode> nodes() const noexcept { return mNodes; }

 private:
  friend class AssetManager;
  friend class GltfParser;

  std::vector<TextureData> mTextures;
  std::vector<MeshData> mMeshes;
  std::vector<SceneNode> mNodes;

  explicit GltfAsset(AssetIdType id, CreateInfo info)
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

  explicit ShaderAsset(AssetIdType id, const CreateInfo& info)
      : Asset(id, info) {}

  void setParsedCode(std::span<u8> code) noexcept {
    mParsedCode.clear();
    mParsedCode.reserve(code.size());
    std::transform(code.begin(), code.end(), std::back_inserter(mParsedCode),
                   [](uint8_t b) { return static_cast<std::byte>(b); });
  }
};
}  // namespace pd