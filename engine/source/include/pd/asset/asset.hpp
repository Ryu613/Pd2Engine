#pragma once

#include "pd/asset/asset_types.hpp"

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

class MeshData {
 public:
  struct SubMesh {};
  explicit MeshData(const DataInfo& info)
      : mDataInfo(info) {}

  DEFAULT_MOVABLE(MeshData);
  DELETE_COPY(MeshData);

  void addSubMesh(const SubMesh& subMesh) { mSubMeshes.push_back(subMesh); }

 private:
  DataInfo mDataInfo;
  std::vector<SubMesh> mSubMeshes;
};

class Asset {
 public:
  struct CreateInfo {
    std::string name;
    std::string path;
    AssetType parseType = AssetType::Gltf;
  };

  ~Asset() = default;
  DEFAULT_MOVABLE(Asset);
  DELETE_COPY(Asset);

  AssetIdType id() const noexcept { return mId; }
  std::span<const TextureData> textures() const noexcept { return mTextures; }
  std::span<const MeshData> meshes() const noexcept { return mMeshes; }
  const CreateInfo& info() const noexcept { return mInfo; }

 private:
  friend class AssetManager;
  friend class GltfParser;

  AssetIdType mId;
  CreateInfo mInfo;
  std::vector<TextureData> mTextures;
  std::vector<MeshData> mMeshes;

  explicit Asset(AssetIdType id, CreateInfo info)
      : mId(std::move(id)),
        mInfo(std::move(info)) {}
};
}  // namespace pd