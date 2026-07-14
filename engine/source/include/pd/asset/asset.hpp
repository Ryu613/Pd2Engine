#pragma once

#include "pd/resource/resource_alias.hpp"

namespace pd {
/**
 * @brief 资产对象，包含资产元数据信息，方便后续运行时使用
 *
 */
class Asset {
 public:
  // 当前为路径字符串
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
  struct CreateInfo {
    std::string name;
    std::string path;
    Type parseType = Type::Gltf;
  };

  Asset() noexcept = default;
  ~Asset() = default;
  DEFAULT_MOVABLE(Asset);
  DELETE_COPY(Asset);

  [[nodiscard]] Asset::IdType getId() const noexcept { return mId; }

  [[nodiscard]] bool isNull() const noexcept { return mId.empty(); }

  [[nodiscard]] std::string getPath() const noexcept { return mInfo.path; }

 private:
  friend class AssetManager;
  friend class GltfParser;
  CreateInfo mInfo;
  IdType mId{};
  // TODO(author): scene node
  //  转换后的资源数据
  std::vector<TextureHandle> mTextures;
  std::vector<MeshHandle> mMeshes;

  explicit Asset(IdType id, CreateInfo info) noexcept
      : mId(std::move(id)),
        mInfo(std::move(info)) {};
};
}  // namespace pd
