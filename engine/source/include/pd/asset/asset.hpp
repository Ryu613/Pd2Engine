#pragma once

#include "pd/core/entity.hpp"
#include "pd/rendering/resource/texture_resource.hpp"
#include "pd/rendering/resource/mesh_resource.hpp"

namespace pd {
enum class AssetError : uint8_t {
  FileNotFound = 1,
  FileLoadError = 2,
  ParseFailed = 3,
  Unknown = 99,
};
/**
 * @brief 资产对象，包含资产元数据信息，方便后续运行时使用
 *
 */
class Asset {
 public:
  // 目前用路径字符串实现id
  using IdType = std::string;
  /**
   * @brief 资产类型
   *
   */
  enum class Type : uint8_t {
    Gltf,
  };
  /**
   * @brief 资产信息，用于描述资产特征，作为资产管理器的参数
   *
   */
  struct Info {
    std::string name;
    std::string path;
    Type parseType = Type::Gltf;
  };

  MOVABLE_ONLY(Asset);
  Asset() noexcept = default;
  ~Asset() = default;

  [[nodiscard]] Asset::IdType getId() const noexcept { return mId; }

  [[nodiscard]] bool isNull() const noexcept { return mId.empty(); }

  [[nodiscard]] Entity getRootEntity() const noexcept { return mRoot; }

  [[nodiscard]] std::string getPath() const noexcept { return mInfo.path; }

 private:
  friend class AssetManager;
  friend class GltfParser;
  Info mInfo;
  IdType mId{};
  bool mLoaded = false;
  Entity mRoot{};
  std::vector<Entity> mEntities;

  std::vector<std::unique_ptr<TextureResource>> mTextures;
  std::vector<std::vector<std::unique_ptr<MeshResource>>> mMeshes;

  explicit Asset(IdType id, Info info) noexcept
      : mId(std::move(id)),
        mInfo(std::move(info)) {};
};
}  // namespace pd
