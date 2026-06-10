#include "pd/asset/parser/gltf_parser.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "pd/platform/file/file_system.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/rendering/resource/texture_resource.hpp"
#include "pd/rendering/resource/mesh_resource.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/parser/mesh_processor.hpp"
#include "pd/scene/component/components.hpp"

namespace pd {
using MeshHandle = ResourceHandle<MeshResource>;
namespace {

MeshResource::Properties convertMeshData(const fastgltf::Primitive& primitive,
                                         const fastgltf::Asset& gltfAsset) noexcept {
  MeshProcessor::Input input{};
  const auto* posIt = primitive.findAttribute("POSITION");
  PD_ASSERT_MSG(posIt != primitive.attributes.end(),
                "gltf primitive is missing POSITION!");
  const auto& positionAccessor = gltfAsset.accessors[posIt->accessorIndex];
  input.positions.reserve(positionAccessor.count);
  fastgltf::iterateAccessor<fastgltf::math::fvec3>(
      gltfAsset, positionAccessor, [&](fastgltf::math::fvec3 pos) {
        input.positions.emplace_back(pos.x(), pos.y(), pos.z());
      });
  const auto* normIt = primitive.findAttribute("NORMAL");
  PD_ASSERT_MSG(normIt != primitive.attributes.end(),
                "GLTF primitive is missing NORMAL!");
  const auto& normalAccessor = gltfAsset.accessors[normIt->accessorIndex];
  input.normals.reserve(normalAccessor.count);
  fastgltf::iterateAccessor<fastgltf::math::fvec3>(
      gltfAsset, normalAccessor, [&](fastgltf::math::fvec3 norm) {
        input.normals.emplace_back(norm.x(), norm.y(), norm.z());
      });
  const auto* uvIt = primitive.findAttribute("TEXCOORD_0");
  if (uvIt != primitive.attributes.end()) {
    const auto& uvAccessor = gltfAsset.accessors[uvIt->accessorIndex];
    input.uvs.reserve(uvAccessor.count);
    fastgltf::iterateAccessor<fastgltf::math::fvec2>(
        gltfAsset, uvAccessor,
        [&](fastgltf::math::fvec2 uv) { input.uvs.emplace_back(uv.x(), uv.y()); });
  }
  const auto* colorIt = primitive.findAttribute("COLOR_0");
  if (colorIt != primitive.attributes.end()) {
    const auto& colorAccessor = gltfAsset.accessors[colorIt->accessorIndex];
    input.colors.reserve(colorAccessor.count);
    fastgltf::iterateAccessor<fastgltf::math::fvec3>(
        gltfAsset, colorAccessor, [&](fastgltf::math::fvec3 color) {
          input.colors.emplace_back(color.x(), color.y(), color.z());
        });
  }
  if (primitive.indicesAccessor.has_value()) {
    const auto& indexAccessor = gltfAsset.accessors[*primitive.indicesAccessor];
    input.indices.reserve(indexAccessor.count);
    fastgltf::iterateAccessor<uint32_t>(gltfAsset, indexAccessor, [&](uint32_t index) {
      input.indices.emplace_back(index);
    });
  }
  return MeshProcessor::process(input);
}
}  // namespace

GltfParser::GltfParser(FileSystem& fs, EntityManager& em, ResourceManager& rm) noexcept
    : mFileSystem(fs),
      mEntityManager(em),
      mResourceManager(rm) {}

std::expected<void, AssetError> GltfParser::parse(Asset& asset) noexcept {
  const auto& assetPath = asset.getPath();
  // 读取gltf文件
  std::filesystem::path gltfFilePath{assetPath};
  // 设置根文件目录
  mBasePath = gltfFilePath.parent_path();

  fastgltf::Parser parser;
  auto data = fastgltf::GltfDataBuffer::FromPath(gltfFilePath);
  if (data.error() != fastgltf::Error::None) {
    log::error("asset cannot be loaded: {}", gltfFilePath.string());
    return std::unexpected(AssetError::FileLoadError);
  }
  auto options =
      fastgltf::Options::LoadExternalBuffers | fastgltf::Options::DecomposeNodeMatrices;
  auto gltfAssetRes = parser.loadGltf(data.get(), gltfFilePath.parent_path(), options);
  if (auto error = gltfAssetRes.error(); error != fastgltf::Error::None) {
    log::error("gltf asset file parse failed: {}", gltfFilePath.string());
    return std::unexpected(AssetError::ParseFailed);
  }
  // 2.1 创建gltf Asset
  auto& gltfAsset = gltfAssetRes.get();
  // 2.2 解析网格数据
  parseMeshes(asset, gltfAsset);
  // 2.3 解析texture(samplers)
  parseTextures(asset, gltfAsset);
  // 2.4 解析material
  parseMaterials(asset, gltfAsset);
  // 2.5 解析scene,目前默认只解析第一个场景
  size_t scene0 = gltfAsset.defaultScene.value_or(0);
  parseScene(asset, gltfAsset, scene0);
  return {};
}

void GltfParser::parseMeshes(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
  auto& meshes = asset.mMeshes;
  meshes.resize(gltfAsset.meshes.size());
  for (size_t i = 0; i < gltfAsset.meshes.size(); ++i) {
    const auto& mesh = gltfAsset.meshes[i];
    meshes[i].reserve(mesh.primitives.size());
    for (const auto& primitive : mesh.primitives) {
      auto meshProps = convertMeshData(primitive, gltfAsset);
      auto newMesh = std::make_unique<MeshResource>(meshProps);
      meshes[i].push_back(std::move(newMesh));
    }
  }
}

void GltfParser::parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
  // 根据材质用途确定纹理格式
  mTextureFormatCache.clear();
  mTextureFormatCache.resize(gltfAsset.textures.size(), TextureFormat::RGBA8_UNORM);
  for (const auto& material : gltfAsset.materials) {
    if (material.pbrData.baseColorTexture.has_value()) {
      mTextureFormatCache[material.pbrData.baseColorTexture->textureIndex] =
          TextureFormat::RGBA8_SRGB;
    }
    if (material.pbrData.metallicRoughnessTexture.has_value()) {
      mTextureFormatCache[material.pbrData.metallicRoughnessTexture->textureIndex] =
          TextureFormat::RGBA8_UNORM;
    }
    if (material.normalTexture.has_value()) {
      mTextureFormatCache[material.normalTexture->textureIndex] =
          TextureFormat::RGBA8_UNORM;
    }
    if (material.occlusionTexture.has_value()) {
      mTextureFormatCache[material.occlusionTexture->textureIndex] =
          TextureFormat::RGBA8_UNORM;
    }
    if (material.emissiveTexture.has_value()) {
      mTextureFormatCache[material.emissiveTexture->textureIndex] =
          TextureFormat::RGBA8_UNORM;
    }
  }
  // 解析，转换纹理，并添加到资产中
  for (size_t i = 0; i < gltfAsset.textures.size(); ++i) {
    const auto& texture = gltfAsset.textures[i];
    if (!texture.imageIndex.has_value()) {
      continue;
    }
    const auto& image = gltfAsset.images[texture.imageIndex.value()];
    std::visit(fastgltf::visitor{
                   [](auto& arg) {
                     log::warn("reach!");
                     PD_ASSERT_MSG(false, "texture parse error!");
                   },
                   [&](const fastgltf::sources::URI& filePath) {
                     PD_ASSERT(filePath.fileByteOffset == 0);
                     PD_ASSERT(filePath.uri.isLocalPath());
                     int width = 0;
                     int height = 0;
                     int nrChannels = 0;
                     const auto path = mBasePath / filePath.uri.path();
                     // 用stb解析图片信息
                     stbi_uc* texels = stbi_load(path.string().c_str(), &width, &height,
                                                 &nrChannels, STBI_rgb_alpha);
                     if (!texels) {
                       PD_ASSERT_MSG(false, "failed to load image!");
                     }
                     // 生成TextureResource
                     TextureResource::Properties props{
                         .name = std::string(image.name),
                         .path = path.string(),
                         .width = static_cast<uint32_t>(width),
                         .height = static_cast<uint32_t>(height),
                         .channel = static_cast<uint32_t>(nrChannels),
                         .format = mTextureFormatCache[i],
                         .pSourceData = texels,
                     };
                     auto texture = std::make_unique<TextureResource>(props);
                     // 加入到asset中
                     asset.mTextures.push_back(std::move(texture));
                     // 清除使用后的image数据
                     stbi_image_free(texels);
                   },
                   [&](const fastgltf::sources::Array& vector) { log::warn("reach!"); },
                   [&](const fastgltf::sources::BufferView& view) {
                     const auto& bufferView = gltfAsset.bufferViews[view.bufferViewIndex];
                     const auto& buffer = gltfAsset.buffers[bufferView.bufferIndex];
                     const auto& data = std::get<fastgltf::sources::Array>(buffer.data);
                     const auto* imagePtr = data.bytes.data() + bufferView.byteOffset;
                     int width = 0;
                     int height = 0;
                     int nrChannels = 0;
                     auto* texels =
                         stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(imagePtr),
                                               bufferView.byteLength, &width, &height,
                                               &nrChannels, STBI_rgb_alpha);
                     if (!texels) {
                       PD_ASSERT_MSG(false, "failed to load image!");
                     }
                     TextureResource::Properties props{
                         .name = std::string(image.name),
                         .width = static_cast<uint32_t>(width),
                         .height = static_cast<uint32_t>(height),
                         .channel = static_cast<uint32_t>(nrChannels),
                         .format = mTextureFormatCache[i],
                         .pSourceData = texels,
                     };
                     auto texture = std::make_unique<TextureResource>(props);

                     asset.mTextures.push_back(std::move(texture));

                     stbi_image_free(texels);
                   },
               },
               image.data);
  }
}

void GltfParser::parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
}

void GltfParser::parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                            size_t gltfSceneIndex) noexcept {
  const auto& gltfScene = gltfAsset.scenes[gltfSceneIndex];
  auto& em = mEntityManager;
  auto& rm = mResourceManager;
  // 根据node数创建对应数量的entity,遍历每个node
  auto entityList = em.createEntity(gltfAsset.nodes.size());
  for (size_t i = 0; i < gltfAsset.nodes.size(); ++i) {
    const auto& node = gltfAsset.nodes[i];
    const auto& trs = std::get<fastgltf::TRS>(node.transform);
    const auto& currentEntity = entityList[i];
    auto entityName =
        node.name.empty() ? std::format("node_{}", i) : std::string(node.name);
    auto position =
        glm::vec3{trs.translation.x(), trs.translation.y(), trs.translation.z()};
    auto rotation =
        glm::quat{trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z()};
    auto scale = glm::vec3{trs.scale.x(), trs.scale.y(), trs.scale.z()};

    em.addComponent<Name>(currentEntity, entityName);
    // 设置每个entity的transform
    em.addComponent<Transform>(currentEntity, position, rotation, scale);
    // 若有mesh，则设置mesh
    if (node.meshIndex.has_value()) {
      const auto& submeshes = asset.mMeshes[*node.meshIndex];
      const auto& gltfMesh = gltfAsset.meshes[*node.meshIndex];
      // 遍历每个mesh的submesh
      // 若此mesh只由一个submesh组成
      if (submeshes.size() == 1) {
        // rm 注册meshResource拿到MeshHandle
        auto meshHandle = rm.registerResource<MeshResource>(*submeshes[0]);
        // em添加MeshHandle作为组件
        em.addComponent<MeshHandle>(currentEntity, meshHandle);
        // TODO(author): 材质组件
        // em.addComponent<Material>(submesh[0]);
      } else {
        auto childEntityList = em.createEntity(submeshes.size());
        for (size_t j = 0; j < submeshes.size(); ++j) {
          const auto& childEntity = childEntityList[j];
          const auto& submesh = submeshes[j];
          auto meshHandle = rm.registerResource<MeshResource>(*submesh);
          em.addComponent<Name>(childEntity, std::format("{}_submesh_{}", entityName, j));
          em.addComponent<MeshHandle>(childEntity, meshHandle);
          // TODO(author): current entity add child
        }
      }
    }
  }
}
}  // namespace pd