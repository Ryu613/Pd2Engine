#include "world.hpp"
#include "pd/scene/scene_manager.hpp"
#include "pd/asset/asset.hpp"
#include "pd/asset/asset_manager.hpp"

#ifndef ASSET_BASE_DIR
#define ASSET_BASE_DIR ./
#endif

namespace sandbox {
Result<void> WorldScene::onLoadScene(SceneManager& sceneManager) noexcept {
  // 1. load assets
  // 1.1 load earth gltf
  auto& assetMgr = sceneManager.getAssetManager();
  Asset::CreateInfo earthInfo{
      .name = "earth",
      .path = ASSET_BASE_DIR "models/earth/scene.gltf",
      .parseType = Asset::Type::Gltf,
  };
  auto earthAssetRes = assetMgr.createAsset(earthInfo);
  if (!earthAssetRes) {
    return make_error<void>(ErrorCode::AssetFileLoadError);
  }
  auto* earthAsset = earthAssetRes.value();
  // 2. create scene graph node using earth asset
  // 2.1 create entity
  auto& entityMgr = sceneManager.getEntityManager();
  auto earthEntity = entityMgr.createEntity();
  // 2.2 create scene node
  // TODO: scene node format: entityid | transform | asset_id
  entityMgr.addComponent<MeshHandle>(earthEntity, earthAsset);
  // 2.3 add earth to scene
  auto& scene = sceneManager.getScene();
  scene.addEntity(earthEntity);
  // 3. place scene object
  // 3.1 set earth transform matrix
  Transform earthTrans{};
  entityMgr.addComponent<Transform>(earthEntity, earthTrans);
  // 4. create and configure camera
  // 4.1 create camera from scene view
  auto& view = scene.createView();
  auto cameraEntity = entityMgr.createEntity();
  view.setMainCamera(cameraEntity);
  // 4.2 set camera parameters
  Camera cam{
      .zNear = 0.1f,
      .zFar = 1000.f,
      .hFov = 0.75f,
      .mAspectRatio = 1024 / 768.f,
  };
  // 4.3 set camera transform matrix
  Transform camTrans{
      .location = math::vec3{0.f, 0.f, -2.f},
  };
  entityMgr.addComponent<Transform>(cameraEntity, camTrans);
  entityMgr.addComponent<Camera>(cameraEntity, cam);
  return {};
}

void WorldScene::onUpdateScene(float deltaTime) noexcept {
  // 1. rotate earth instance by delta
}

Result<void> WorldScene::onUnloadScene() noexcept {
  // do nothing(let engine clear all resources)
  return {};
}
}  // namespace sandbox