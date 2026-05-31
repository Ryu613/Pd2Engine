#pragma once

#include "pd/core/entity.hpp"
#include "pd/rendering/resource/texture_resource.hpp"

namespace pd {
/**
 * @brief 资产对象，包含资产元数据信息，方便后续运行时使用
 *
 */
class Asset {
 public:
  // 目前用路径字符串实现id
  using IdType = std::string;
  /**
   * @brief 资产信息，用于描述资产特征，作为资产管理器的参数
   *
   */
  struct Info {
    std::string name;
    std::string path;
  };
  /**
   * @brief 资产类型
   *
   */
  enum class Type : uint8_t {
    Gltf = 1,
  };

  MOVABLE_ONLY(Asset);
  Asset() noexcept = default;
  ~Asset() = default;

  [[nodiscard]] Asset::IdType getId() const noexcept { return mId; }

  [[nodiscard]] bool isNull() const noexcept { return mId.empty(); }

  [[nodiscard]] Entity getRootEntity() const noexcept { return mRoot; }

  void addTextureResource(std::unique_ptr<TextureResource> resource) noexcept {
    mTextures.emplace_back(std::move(resource));
  }

 private:
  friend class AssetManager;
  IdType mId{};
  bool mLoaded = false;
  Entity mRoot{};
  std::vector<Entity> mEntities;
  std::vector<std::unique_ptr<TextureResource>> mTextures;

  // 只有AssetManager能构造
  explicit Asset(IdType id) noexcept
      : mId(std::move(id)) {};

  void setId(const IdType& id) noexcept { mId = id; }
};
}  // namespace pd
