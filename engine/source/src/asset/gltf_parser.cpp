#include "pd/asset/parser/gltf_parser.hpp"

#include "pd/core/math/math.hpp"
#include "pd/asset/asset.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"

namespace pd {
namespace {
math::mat4 getGltfNodeLocalTransform(const fastgltf::Node& node) {
  return {};
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
    LOG_ERROR("gltf asset file parse failed: {}, reason: {}", gltfFilePath.string(),
              fastgltf::getErrorMessage(error));
    return make_error<void>(ErrorCode::AssetParseFailed);
  }
  // 2.1 创建gltf Asset
  auto& gltfAsset = gltfAssetRes.get();
  size_t scene0 = gltfAsset.defaultScene.value_or(0);
  parseScene(asset, gltfAsset, scene0);
  // 2.2 解析网格数据
  // parseMeshes(asset, gltfAsset);
  // 2.3 解析texture(samplers)
  //   parseTextures(asset, gltfAsset);
  //   // 2.4 解析material
  //   parseMaterials(asset, gltfAsset);
  //   // 2.5 解析scene,目前默认只解析第一个场景
  //   size_t scene0 = gltfAsset.defaultScene.value_or(0);
  //   parseScene(asset, gltfAsset, scene0);
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

void GltfParser::parseScene(Asset& asset, const fastgltf::Asset& gltfAsset,
                            size_t sceneIndex) noexcept {
  std::function<void(u32, const math::mat4&)> traverseNode =
      [&](u32 nodeIndex, const math::mat4& parentTransform) {
        const fastgltf::Node& gltfNode = gltfAsset.nodes[nodeIndex];
        const math::mat4 worldTransform = parentTransform * getGltfNodeLocalTransform(gltfNode);

        // todo: handle camera data

        // 处理node的mesh数据
        if(gltfNode.meshIndex.has_value()) {

        }
      };
  for (const int nodeIndex : gltfAsset.scenes[sceneIndex].nodeIndices) {
    traverseNode(nodeIndex, math::mat4{1.0f});
  }
}
}  // namespace pd