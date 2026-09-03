#include "pd/asset/parser/gltf_parser.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "pd/platform/file/file_system.hpp"

namespace pd {
namespace {

void convertMeshData(MeshData& meshData, const fastgltf::Mesh& mesh,
                     const fastgltf::Asset& gltfAsset) noexcept {
  meshData.name = mesh.name;
  meshData.primitives.reserve(mesh.primitives.size());
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    auto& primitive = mesh.primitives[i];
    // MeshProcessor::Input input{};
    meshData.primitives.emplace_back(MeshData::Primitive{});
    const auto* posIt = primitive.findAttribute("POSITION");
    PD_ASSERT_MSG(posIt != primitive.attributes.end(), "gltf primitive is missing POSITION!");
    const auto& positionAccessor = gltfAsset.accessors[posIt->accessorIndex];
    // input.positions.reserve(positionAccessor.count);
    meshData.vertices.resize(meshData.vertices.size() + positionAccessor.count);
    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        gltfAsset, positionAccessor, [&](fastgltf::math::fvec3 pos, size_t idx) {
          meshData.vertices[idx].pos = {pos.x(), pos.y(), pos.z()};
        });
    auto& parsedPrimitive = meshData.primitives.back();
    parsedPrimitive.indexCount = i == 0 ? 0 : positionAccessor.count;
    parsedPrimitive.indexOffset = sizeof(Vertex) * positionAccessor.count;
    const auto* normIt = primitive.findAttribute("NORMAL");
    PD_ASSERT_MSG(normIt != primitive.attributes.end(), "GLTF primitive is missing NORMAL!");
    const auto& normalAccessor = gltfAsset.accessors[normIt->accessorIndex];
    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
        gltfAsset, normalAccessor, [&](fastgltf::math::fvec3 norm, size_t idx) {
          meshData.vertices[idx].normal = {norm.x(), norm.y(), norm.z()};
        });

    const auto* uvIt = primitive.findAttribute("TEXCOORD_0");
    if (uvIt != primitive.attributes.end()) {
      const auto& uvAccessor = gltfAsset.accessors[uvIt->accessorIndex];
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
          gltfAsset, uvAccessor, [&](fastgltf::math::fvec2 uv, size_t idx) {
            meshData.vertices[idx].uv = {uv.x(), uv.y()};
          });
    }

    // const auto* colorIt = primitive.findAttribute("COLOR_0");
    // if (colorIt != primitive.attributes.end()) {
    //   const auto& colorAccessor = gltfAsset.accessors[colorIt->accessorIndex];
    //   fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
    //       gltfAsset, colorAccessor, [&](fastgltf::math::fvec3 color, size_t idx) {
    //         meshData.vertices[idx].tangent = {color.x(), color.y(), color.z()};
    //       });
    // }
    const auto* tangentIt = primitive.findAttribute("TANGENT");
    if (tangentIt != primitive.attributes.end()) {
      const auto& tanAccessor = gltfAsset.accessors[tangentIt->accessorIndex];
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
          gltfAsset, tanAccessor, [&](fastgltf::math::fvec4 tan, size_t idx) {
            meshData.vertices[idx].tangent = {tan.x(), tan.y(), tan.z(), tan.w()};
          });
    }
    if (primitive.indicesAccessor.has_value()) {
      const auto& indexAccessor = gltfAsset.accessors[*primitive.indicesAccessor];
      meshData.indices.reserve(indexAccessor.count);
      fastgltf::iterateAccessor<uint32_t>(
          gltfAsset, indexAccessor, [&](uint32_t index) { meshData.indices.emplace_back(index); });
    }
    // return MeshProcessor::process(input);
  }
}

Node makeParsedNode(const math::mat4& transform) {
  Node parsedNode{
      .location = math::vec3{transform[3]},
      .scale =
          math::vec3{glm::length(glm::vec3{transform[0]}), glm::length(glm::vec3{transform[1]}),
                     glm::length(glm::vec3{transform[2]})},
  };
  // 参考自siggraph2026_vulkan
  glm::mat3 rotationMatrix{1.0F};
  for (glm::length_t column = 0; column < 3; ++column) {
    const float axisScale = parsedNode.scale[column];
    if (axisScale > 0.0F) {
      rotationMatrix[column] = glm::vec3{transform[column]} / axisScale;
    }
  }
  parsedNode.rotationAuler = glm::eulerAngles(glm::quat_cast(rotationMatrix));

  return parsedNode;
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
    auto parsedMesh = std::make_unique<MeshData>();
    parsedMesh->id = i;
    convertMeshData(*parsedMesh, gltfAsset.meshes[i], gltfAsset);
    meshes.push_back(std::move(parsedMesh));
  }
}

void GltfParser::parseTextures(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
  /*
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
      */
}

void GltfParser::parseMaterials(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {}

void GltfParser::parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                            size_t gltfSceneIndex) noexcept {
  const auto& gltfScene = gltfAsset.scenes[gltfSceneIndex];
  asset.mNodes.reserve(gltfAsset.nodes.size());
  std::function<void(u32, std::string_view nodeName, const math::mat4&)> traverseNode;
  traverseNode = [&asset, &gltfAsset, &traverseNode](u32 nodeIndex, std::string_view nodeName,
                                                     const math::mat4& parentTransform) {
    const auto& gltfNode = gltfAsset.nodes[nodeIndex];
    const auto& trs = std::get<fastgltf::TRS>(gltfNode.transform);
    auto translation = glm::vec3{trs.translation.x(), trs.translation.y(), trs.translation.z()};
    auto rotation =
        glm::quat{trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z()};
    auto scale = glm::vec3{trs.scale.x(), trs.scale.y(), trs.scale.z()};
    auto localTransform = glm::translate(glm::mat4{1.0F}, translation) * glm::mat4_cast(rotation) *
                          glm::scale(glm::mat4{1.0F}, scale);
    const auto worldTransform = parentTransform * localTransform;

    // todo: parse camera data
    if (gltfNode.meshIndex.has_value()) {
      const auto& meshIndex = gltfNode.meshIndex.value();
      const auto& mesh = gltfAsset.meshes[meshIndex];
      const u32 primitiveCount = mesh.primitives.size();
      for (u32 primitiveIndex = 0; primitiveIndex < primitiveCount; ++primitiveIndex) {
        const auto& primitive = mesh.primitives[primitiveIndex];
        PD_ASSERT_MSG(primitive.type == fastgltf::PrimitiveType::Triangles,
                      "gltf mesh primitive type not supported");
        Node parsedNode = makeParsedNode(worldTransform);
        parsedNode.name = nodeName;
        // mesh索引与parsed mesh的索引应该是一致的
        parsedNode.meshIndex = meshIndex;
        asset.mNodes.push_back(parsedNode);
        // 递归子节点
        for (const int childIndex : gltfNode.children) {
          auto childNodeName = std::format("node_{}_child_{}", nodeName, childIndex);
          // no parent & children index?
          traverseNode(childIndex, childNodeName, worldTransform);
        }
      }
    }
  };
  for (uint32_t nodeIndex = 0; nodeIndex < gltfAsset.nodes.size(); ++nodeIndex) {
    traverseNode(nodeIndex, "node_root", math::mat4{1.0f});
  }
}
}  // namespace pd