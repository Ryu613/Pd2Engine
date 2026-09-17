#include "pd/asset/parser/gltf_parser.hpp"

#include "pd/core/math/math.hpp"
#include "pd/asset/asset.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"

namespace pd {
namespace {

math::mat4 getGltfNodeLocalTransform(const fastgltf::Node& node) {
  // todo
  return {};
}

SceneNode makeParsedNode(const u32 nodeIndex, const math::mat4& transform) {
  SceneNode node{
      .name = std::format("node_{}", nodeIndex),
      .pos = math::vec3{transform[3]},
      .scale = math::vec3{glm::length(glm::vec3{transform[0]}), glm::length(glm::vec3{transform[1]}),
                          glm::length(glm::vec3{transform[2]})},
  };
  // 参考自siggraph2026_vulkan
  glm::mat3 rotationMatrix{1.0F};
  for (glm::length_t column = 0; column < 3; ++column) {
    const float axisScale = node.scale[column];
    if (axisScale > 0.0F) {
      rotationMatrix[column] = glm::vec3{transform[column]} / axisScale;
    }
  }
  node.eulerAngles = glm::eulerAngles(glm::quat_cast(rotationMatrix));

  return node;
}

}  // namespace
GltfParser::GltfParser(IFileSystem* fs)
    : mFs(fs) {}

GltfParser::~GltfParser() {}

Result<void> GltfParser::parse(Asset& asset) noexcept {
  const auto& assetPath = asset.info().path;
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
    LOG_ERROR("gltf asset file parse failed: {}, reason: {}", gltfFilePath.string(), fastgltf::getErrorMessage(error));
    return make_error<void>(ErrorCode::AssetParseFailed);
  }
  // 2.1 创建gltf Asset
  auto& gltfAsset = gltfAssetRes.get();
  size_t scene0 = gltfAsset.defaultScene.value_or(0);
  if (auto res = parseScene(asset, gltfAsset, scene0); !res) {
    return res;
  }
  // 2.2 解析网格数据
  // parseMeshes(asset, gltfAsset);
  // 2.3 解析texture(samplers)
  //   parseTextures(asset, gltfAsset);
  //   // 2.4 解析material
  //   parseMaterials(asset, gltfAsset);
  //   // 2.5 解析scene,目前默认只解析第一个场景
  //   size_t scene0 = gltfAsset.defaultScene.value_or(0);
  return {};
}

// void GltfParser::parseMeshes(Asset& asset, const fastgltf::Asset& gltfAsset) noexcept {
//   auto& meshes = asset.mMeshes;
//   meshes.reserve(gltfAsset.meshes.size());
//   for (size_t i = 0; i < gltfAsset.meshes.size(); ++i) {
//     auto& meshName = gltfAsset.meshes[i].name;
//     DataInfo meshInfo{
//         .dataId = static_cast<u32>(i),
//         .name = meshName.empty() ? std::format("mesh_{}", i) : meshName.c_str(),
//     };
//     MeshData meshData{meshInfo};
//     // convertMeshData(*parsedMesh, gltfAsset.meshes[i], gltfAsset);
//     meshes.push_back(std::move(meshData));
//   }
// }

Result<void> GltfParser::parseScene(Asset& asset, const fastgltf::Asset& gltfAsset, size_t sceneIndex) noexcept {
  std::function<void(u32, const math::mat4&)> traverseNode = [&](u32 nodeIndex, const math::mat4& parentTransform) {
    const fastgltf::Node& gltfNode = gltfAsset.nodes[nodeIndex];
    const math::mat4 worldTransform = parentTransform * getGltfNodeLocalTransform(gltfNode);

    // todo: handle camera data

    // 处理node的mesh数据
    if (gltfNode.meshIndex.has_value()) {
      const fastgltf::Mesh& mesh = gltfAsset.meshes[gltfNode.meshIndex.value()];
      const u32 primitiveCount = mesh.primitives.size();
      MeshData meshData{
          .dataInfo =
              {
                  .dataId = nodeIndex,
                  .name = std::format("mesh_{}", nodeIndex),
              },
      };
      meshData.subMeshes.resize(primitiveCount);
      for (u32 primitiveIndex = 0; primitiveIndex < primitiveCount; ++primitiveIndex) {
        const auto& primitive = mesh.primitives[primitiveIndex];
        if (primitive.type != fastgltf::PrimitiveType::Triangles) {
          return make_error<void>(ErrorCode::AssetParseFailed, "primitive type not supported!");
        }
        auto& submesh = meshData.subMeshes[primitiveIndex];
        submesh.name = std::format("{}_{}", meshData.dataInfo.name, primitiveIndex);
        // positions
        const auto positionIt = primitive.findAttribute("POSITION");
        PD_ASSERT_MSG(positionIt != primitive.attributes.end(), "gltf primitive is missing POSITION!");
        const auto& positionAccessor = gltfAsset.accessors[positionIt->accessorIndex];
        // input.positions.reserve(positionAccessor.count);
        submesh.vertices.resize(positionAccessor.count);
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            gltfAsset, positionAccessor, [&](fastgltf::math::fvec3 pos, size_t idx) {
              submesh.vertices[idx].position = {pos.x(), pos.y(), pos.z()};
            });
        const auto* uvIt = primitive.findAttribute("TEXCOORD_0");
        if (uvIt != primitive.attributes.end()) {
          const auto& uvAccessor = gltfAsset.accessors[uvIt->accessorIndex];
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
              gltfAsset, uvAccessor,
              [&](fastgltf::math::fvec2 uv, size_t idx) { submesh.vertices[idx].uv = {uv.x(), uv.y()}; });
        }
        const auto normalIt = primitive.findAttribute("NORMAL");
        PD_ASSERT_MSG(normalIt != primitive.attributes.end(), "GLTF primitive is missing NORMAL!");
        const auto& normalAccessor = gltfAsset.accessors[normalIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            gltfAsset, normalAccessor, [&](fastgltf::math::fvec3 norm, size_t idx) {
              submesh.vertices[idx].normal = {norm.x(), norm.y(), norm.z()};
            });
        const auto tangentIt = primitive.findAttribute("TANGENT");
        if (tangentIt != primitive.attributes.end()) {
          const auto& tanAccessor = gltfAsset.accessors[tangentIt->accessorIndex];
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
              gltfAsset, tanAccessor, [&](fastgltf::math::fvec4 tan, size_t idx) {
                submesh.vertices[idx].tangent = {tan.x(), tan.y(), tan.z(), tan.w()};
              });
        }
        if (primitive.indicesAccessor.has_value()) {
          const auto& indexAccessor = gltfAsset.accessors[*primitive.indicesAccessor];
          submesh.indices.reserve(indexAccessor.count);
          fastgltf::iterateAccessor<uint32_t>(gltfAsset, indexAccessor,
                                              [&](uint32_t index) { submesh.indices.emplace_back(index); });
        }
        // 处理scene node
        const auto& gltfNode = gltfAsset.nodes[nodeIndex];
        const auto& trs = std::get<fastgltf::TRS>(gltfNode.transform);
        auto translation = math::vec3{trs.translation.x(), trs.translation.y(), trs.translation.z()};
        auto rotation = math::quat{trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z()};
        auto scale = math::vec3{trs.scale.x(), trs.scale.y(), trs.scale.z()};
        auto localTransform = glm::translate(glm::mat4{1.0F}, translation) * glm::mat4_cast(rotation) *
                              glm::scale(glm::mat4{1.0F}, scale);
        const auto worldTransform = parentTransform * localTransform;
        auto node = makeParsedNode(nodeIndex, worldTransform);
        node.meshId = primitiveIndex;
        asset.mNodes.push_back(node);
      }
    }
    for (const int childIndex : gltfNode.children) {
      traverseNode(childIndex, worldTransform);
    }
  };
  for (const int nodeIndex : gltfAsset.scenes[sceneIndex].nodeIndices) {
    traverseNode(nodeIndex, math::mat4{1.0f});
  }

  return {};
}
}  // namespace pd