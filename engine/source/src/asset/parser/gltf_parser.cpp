#include "pd/asset/parser/gltf_parser.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "pd/platform/file/file_system.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/resource/resource_alias.hpp"
#include "pd/asset/parser/mesh_processor.hpp"

namespace pd {
namespace {

MeshPrimitive convertMeshData(const fastgltf::Primitive& primitive,
                              const fastgltf::Asset& gltfAsset) noexcept {
  MeshProcessor::Input input{};
  const auto* posIt = primitive.findAttribute("POSITION");
  PD_ASSERT_MSG(posIt != primitive.attributes.end(), "gltf primitive is missing POSITION!");
  const auto& positionAccessor = gltfAsset.accessors[posIt->accessorIndex];
  input.positions.reserve(positionAccessor.count);
  fastgltf::iterateAccessor<fastgltf::math::fvec3>(
      gltfAsset, positionAccessor,
      [&](fastgltf::math::fvec3 pos) { input.positions.emplace_back(pos.x(), pos.y(), pos.z()); });
  const auto* normIt = primitive.findAttribute("NORMAL");
  PD_ASSERT_MSG(normIt != primitive.attributes.end(), "GLTF primitive is missing NORMAL!");
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
    fastgltf::iterateAccessor<uint32_t>(gltfAsset, indexAccessor,
                                        [&](uint32_t index) { input.indices.emplace_back(index); });
  }
  return MeshProcessor::process(input);
}

void copySourceData(std::vector<uint8_t>& target, const void* pSource, size_t size) {
  if (pSource == nullptr) {
    return;
  }
  const auto* pBegin = static_cast<const uint8_t*>(pSource);
  const auto* pEnd = pBegin + size;

  target.assign(pBegin, pEnd);
}
}  // namespace

GltfParser::GltfParser(IFileSystem* fs, ResourceManager* rm) noexcept
    : mFileSystem(fs),
      mResourceManager(rm) {}

Result<void> GltfParser::parse(Asset& asset) noexcept {
  const auto& assetPath = asset.getPath();
  // 读取gltf文件
  std::filesystem::path gltfFilePath{assetPath};
  // 设置根文件目录
  mBasePath = gltfFilePath.parent_path();

  constexpr auto supportedExtensions =
      fastgltf::Extensions::KHR_mesh_quantization | fastgltf::Extensions::KHR_texture_transform |
      fastgltf::Extensions::KHR_materials_variants | fastgltf::Extensions::KHR_materials_specular;
  fastgltf::Parser parser(supportedExtensions);
  auto data = fastgltf::GltfDataBuffer::FromPath(gltfFilePath);
  if (data.error() != fastgltf::Error::None) {
    LOG_ERROR("asset cannot be loaded: {}", gltfFilePath.string());
    return make_error<void>(ErrorCode::AssetFileLoadError);
  }
  auto options = fastgltf::Options::LoadExternalBuffers | fastgltf::Options::DecomposeNodeMatrices;
  auto gltfAssetRes = parser.loadGltf(data.get(), gltfFilePath.parent_path(), options);
  if (auto error = gltfAssetRes.error(); error != fastgltf::Error::None) {
    LOG_ERROR("gltf asset file parse failed: {}, reason: {}", gltfFilePath.string(),
              fastgltf::getErrorMessage(error));
    return make_error<void>(ErrorCode::AssetParseFailed);
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
  meshes.reserve(gltfAsset.meshes.size());
  for (size_t i = 0; i < gltfAsset.meshes.size(); ++i) {
    const auto& mesh = gltfAsset.meshes[i];
    // meshes[i].reserve(mesh.primitives.size());
    {
      auto* newMesh = new MeshResource(mesh.name.c_str());
      for (const auto& primitive : mesh.primitives) {
        auto meshPrimitive = convertMeshData(primitive, gltfAsset);
        newMesh->addPrimitive(meshPrimitive);
      }
      auto handle = mResourceManager->registerResource<MeshResource_t>(std::move(*newMesh));
      meshes.push_back(handle);
    }
  }
}

void GltfParser::parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
  // 根据材质用途确定纹理格式
  mTextureFormatCache.clear();
  mTextureFormatCache.resize(gltfAsset.textures.size(), TextureFormat::RGBA8Unorm);
  for (const auto& material : gltfAsset.materials) {
    if (material.pbrData.baseColorTexture.has_value()) {
      mTextureFormatCache[material.pbrData.baseColorTexture->textureIndex] =
          TextureFormat::RGBA8SRGB;
    }
    if (material.pbrData.metallicRoughnessTexture.has_value()) {
      mTextureFormatCache[material.pbrData.metallicRoughnessTexture->textureIndex] =
          TextureFormat::RGBA8Unorm;
    }
    if (material.normalTexture.has_value()) {
      mTextureFormatCache[material.normalTexture->textureIndex] = TextureFormat::RGBA8Unorm;
    }
    if (material.occlusionTexture.has_value()) {
      mTextureFormatCache[material.occlusionTexture->textureIndex] = TextureFormat::RGBA8Unorm;
    }
    if (material.emissiveTexture.has_value()) {
      mTextureFormatCache[material.emissiveTexture->textureIndex] = TextureFormat::RGBA8Unorm;
    }
  }
  // 解析，转换纹理，并添加到资产中
  for (size_t i = 0; i < gltfAsset.textures.size(); ++i) {
    const auto& texture = gltfAsset.textures[i];
    if (!texture.imageIndex.has_value()) {
      continue;
    }
    const auto& image = gltfAsset.images[texture.imageIndex.value()];
    std::visit(
        fastgltf::visitor{
            [](auto& arg) {
              LOG_WARN("reach!");
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
              stbi_uc* texels =
                  stbi_load(path.string().c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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
              };
              // copy data
              size_t dataSize = static_cast<size_t>(props.width) * props.height * props.channel;
              std::vector<uint8_t> copiedData;
              copiedData.reserve(dataSize);
              copySourceData(copiedData, texels, dataSize);

              auto* newTexture = new TextureResource(props, copiedData);
              auto handle =
                  mResourceManager->registerResource<TextureResource_t>(std::move(*newTexture));
              // 加入到asset中
              asset.mTextures.push_back(handle);
              // 清除使用后的image数据
              stbi_image_free(texels);
            },
            [&](const fastgltf::sources::Array& vector) { LOG_WARN("reach!"); },
            [&](const fastgltf::sources::BufferView& view) {
              const auto& bufferView = gltfAsset.bufferViews[view.bufferViewIndex];
              const auto& buffer = gltfAsset.buffers[bufferView.bufferIndex];
              const auto& data = std::get<fastgltf::sources::Array>(buffer.data);
              const auto* imagePtr = data.bytes.data() + bufferView.byteOffset;
              int width = 0;
              int height = 0;
              int nrChannels = 0;
              const auto path = mBasePath;
              auto* texels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(imagePtr),
                                                   bufferView.byteLength, &width, &height,
                                                   &nrChannels, STBI_rgb_alpha);
              if (!texels) {
                PD_ASSERT_MSG(false, "failed to load image!");
              }
              TextureResource::Properties props{
                  .name = std::string(image.name),
                  .path = path.string(),
                  .width = static_cast<uint32_t>(width),
                  .height = static_cast<uint32_t>(height),
                  .channel = static_cast<uint32_t>(nrChannels),
                  .format = mTextureFormatCache[i],
              };
              // copy data
              size_t dataSize = static_cast<size_t>(props.width) * props.height * props.channel;
              std::vector<uint8_t> copiedData;
              copiedData.reserve(dataSize);
              copySourceData(copiedData, texels, dataSize);

              auto* newTexture = new TextureResource(props, copiedData);
              auto handle =
                  mResourceManager->registerResource<TextureResource_t>(std::move(*newTexture));

              asset.mTextures.push_back(handle);

              stbi_image_free(texels);
            },
        },
        image.data);
  }
}

void GltfParser::parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {}

void GltfParser::parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                            size_t gltfSceneIndex) noexcept {
  const auto& gltfScene = gltfAsset.scenes[gltfSceneIndex];
  asset.mSceneNodes.reserve(gltfAsset.nodes.size());
  for (uint32_t nodeIndex = 0; nodeIndex < gltfAsset.nodes.size(); ++nodeIndex) {
    const auto& node = gltfAsset.nodes[nodeIndex];
    const auto& trs = std::get<fastgltf::TRS>(node.transform);
    Node newNode{
        .name = node.name.empty() ? std::format("node_{}", nodeIndex) : std::string(node.name),
        .location = glm::vec3{trs.translation.x(), trs.translation.y(), trs.translation.z()},
        .rotation =
            glm::quat{trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z()},
        .scale = glm::vec3{trs.scale.x(), trs.scale.y(), trs.scale.z()},
        .meshIndex =
            node.meshIndex ? static_cast<uint32_t>(node.meshIndex.value()) : Asset::defaultId,
    };
    asset.mSceneNodes.push_back(std::move(newNode));
  }
  //   auto& em = mEntityManager;
  //   auto& rm = mResourceManager;
  //   // 根据node数创建对应数量的entity,遍历每个node
  //   auto entityList = em.createEntity(gltfAsset.nodes.size());
  //   for (size_t i = 0; i < gltfAsset.nodes.size(); ++i) {
  //     const auto& node = gltfAsset.nodes[i];
  //     const auto& trs = std::get<fastgltf::TRS>(node.transform);
  //     const auto& currentEntity = entityList[i];
  //     auto entityName =
  //         node.name.empty() ? std::format("node_{}", i) : std::string(node.name);
  //     auto position =
  //         glm::vec3{trs.translation.x(), trs.translation.y(), trs.translation.z()};
  //     auto rotation =
  //         glm::quat{trs.rotation.w(), trs.rotation.x(), trs.rotation.y(),
  //         trs.rotation.z()};
  //     auto scale = glm::vec3{trs.scale.x(), trs.scale.y(), trs.scale.z()};

  //     em.addComponent<Name>(currentEntity, entityName);
  //     // 设置每个entity的transform
  //     em.addComponent<Transform>(currentEntity, position, rotation, scale);
  //     // 若有mesh，则设置mesh
  //     if (node.meshIndex.has_value()) {
  //       const auto& submeshes = asset.mMeshes[*node.meshIndex];
  //       const auto& gltfMesh = gltfAsset.meshes[*node.meshIndex];
  //       // 遍历每个mesh的submesh
  //       // 若此mesh只由一个submesh组成
  //       if (submeshes.size() == 1) {
  //         // rm 注册meshResource拿到MeshHandle
  //         auto meshHandle = rm.registerResource<MeshResource>(*submeshes[0]);
  //         // em添加MeshHandle作为组件
  //         em.addComponent<MeshHandle>(currentEntity, meshHandle);
  //         // TODO(ryu613): 材质组件
  //         // em.addComponent<Material>(submesh[0]);
  //       } else {
  //         auto childEntityList = em.createEntity(submeshes.size());
  //         for (size_t j = 0; j < submeshes.size(); ++j) {
  //           const auto& childEntity = childEntityList[j];
  //           const auto& submesh = submeshes[j];
  //           auto meshHandle = rm.registerResource<MeshResource>(*submesh);
  //           em.addComponent<Name>(childEntity, std::format("{}_submesh_{}", entityName,
  //           j)); em.addComponent<MeshHandle>(childEntity, meshHandle);
  //           // TODO(ryu613): current entity add child
  //         }
  //       }
  //     }
  //}
}
}  // namespace pd