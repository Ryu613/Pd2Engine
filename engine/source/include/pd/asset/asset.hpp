#pragma once

#include "pd/resource/resource_types.hpp"

namespace pd {
struct Node {
  std::string name;
  math::vec3 location{0.0f, 0.0f, 0.0f};
  math::vec3 rotationAuler{0.0f, 0.0f, 0.0f};
  math::vec3 scale{1.0f, 1.0f, 1.0f};
  u32 meshIndex = u32_max;
  u32 parentIndex = u32_max;
  u32 firstChild = u32_max;
  u32 nextSibling = u32_max;
};
/**
 * @brief 资产对象，包含资产元数据信息，方便后续运行时使用
 *
 */
class Asset {
 public:
  using IdType = u64;
  static inline u64 nullId = u64_max;
  /**
   * @brief 资产类型
   *
   */
  enum class Type : u8 {
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

  struct MaterialInfo {};

  Asset() noexcept = default;
  ~Asset() = default;
  DEFAULT_MOVABLE(Asset);
  DELETE_COPY(Asset);

  void releaseData() noexcept {
    mMeshes.clear();
    mMeshes.shrink_to_fit();
  }

  [[nodiscard]] Asset::IdType getId() const noexcept { return mId; }

  [[nodiscard]] bool isNull() const noexcept { return mId == nullId; }

  [[nodiscard]] std::string getPath() const noexcept { return mInfo.path; }

  [[nodiscard]] const auto& getMeshes() const noexcept { return mMeshes; }

  [[nodiscard]] const auto& getTextures() const noexcept { return mTextures; }

  [[nodiscard]] const auto& getNodes() const noexcept { return mNodes; }

  [[nodiscard]] CreateInfo getCreateInfo() const noexcept { return mInfo; }

 private:
  friend class GltfParser;
  friend class AssetManager;

  IdType mId;
  CreateInfo mInfo;

  std::vector<std::unique_ptr<MeshData>> mMeshes;
  std::vector<TextureInfo> mTextures;
  std::vector<Node> mNodes;

  explicit Asset(IdType id, CreateInfo info) noexcept
      : mId(std::move(id)),
        mInfo(std::move(info)) {};
};
}  // namespace pd
