#include "catch2/catch_test_macros.hpp"

#include "pd/core/config.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"
#include "pd/backend/command_recorder.hpp"
#include "pd/asset/compiler/shader_compiler.hpp"
#include "pd/scene/camera.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/resource/resource_manager.hpp"

#include "pd/core/math/math.hpp"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

TEST_CASE("core_render_cmds", "backend_vulkan") {  // skip
  using namespace pd;
  PlatformConfig platformCfg{
      .window =
          {
              .width = 1024,
              .height = 768,
          },
  };

  Platform platform(platformCfg);
  REQUIRE(platform.init());

  REQUIRE(platform.windowSystem().createWindow());

  BackendConfig backendCfg{
      .windowHandle = platform.windowSystem().nativeWindowHandle(),
      .width = platformCfg.window.width,
      .height = platformCfg.window.height,
      .enableDebug = true,
  };
  Backend backend;
  REQUIRE(backend.init(backendCfg));
  // test pipeline
  auto& fs = platform.fileSystem();
  std::string filePath = ASSET_DIR "shader/pyramid/pyramid.slang";
  auto shaderCode = fs.readFileBinary(filePath);
  // return spir-v code, and reflect info
  ShaderCompiler shaderCompiler;
  auto shaderDataRes = shaderCompiler.compile({
      .moduleName = "pyramid",
      .modulePath = filePath,
      .code = std::as_bytes(std::span{shaderCode}),
  });
  REQUIRE(shaderDataRes);
  auto& shaderData = shaderDataRes.value();

  // create shader module
  auto vertFragShader = backend.createShaderModule({
      .spirCode = std::as_bytes(std::span{shaderData.spirvCode}),
  });

  // bind group layout
  // set0: ubo
  auto setLayout = backend.createBindingLayout({
      .debugName = "box binding",
      .bindings = {{
          .slot = 0,
          .count = 1,
          .resourceType = ResourceBindingType::UniformBuffer,
          .shaderStages = {ShaderStage::Vertex},
      }},
  });
  // set1: ssbo
  auto setLayoutSSBO = backend.createBindingLayout({
      .debugName = "box binding",
      .bindings = {{
          .slot = 0,
          .count = 1,
          .resourceType = ResourceBindingType::StorageBuffer,
          .shaderStages = {ShaderStage::Vertex},
      }},
  });

  // pipeline layout
  auto layout = backend.createPipelineLayout({
      .debugName = "pyramid layout",
      .bindGroupLayouts = {setLayout, setLayoutSSBO},
  });
  REQUIRE(layout);

  GraphicsPipelineCreateDesc pipelineDesc{
      .debugName = "pyramid",
      .layout = layout,
      .vertexOptions =
          {
              .vertexLayouts = {{
                  .binding = 0,
                  .stride = sizeof(pd::Vertex),
              }},
              .attributes =
                  {
                      {.location = 0,
                       .binding = 0,
                       .format = VertexFormat::RGB32Sfloat,
                       .offset = offsetof(pd::Vertex, position)},
                      {.location = 1,
                       .binding = 0,
                       .format = VertexFormat::RG32Sfloat,
                       .offset = offsetof(pd::Vertex, uv)},
                      {.location = 2,
                       .binding = 0,
                       .format = VertexFormat::RGB32Sfloat,
                       .offset = offsetof(pd::Vertex, normal)},
                      {.location = 3,
                       .binding = 0,
                       .format = VertexFormat::RG32Sfloat,
                       .offset = offsetof(pd::Vertex, tangent)},
                  },
          },
  };
  pipelineDesc.shaderModules.push_back(vertFragShader);
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Vertex,
      .entryPoint = "vertMain",
  });
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Fragment,
      .entryPoint = "fragMain",
  });

  auto pipeline = backend.createGraphicsPipeline(pipelineDesc);
  REQUIRE(pipeline);

  // buffers
  // front face color is red!
  std::array<Vertex, 5> vertices;
  vertices[0] = {
      math::vec3{0.0f, 0.5f, 0.0f},
      math::vec2{0.0f, 0.0f},
      math::vec3{1.0f, 0.0f, 0.0f},
  };
  vertices[1] = {
      math::vec3{0.5f, -0.5f, -0.5f},
      math::vec2{1.0f, 1.0f},
      math::vec3{0.0f, 0.0f, 1.0f},
  };
  vertices[2] = {
      math::vec3{-0.5f, -0.5f, -0.5f},
      math::vec2{0.0f, 1.0f},
      math::vec3{0.0f, 1.0f, 1.0f},
  };

  vertices[3] = {
      math::vec3{0.5f, -0.5f, 0.5f},
      math::vec2{1.0f, 1.0},
      math::vec3{1.0f, 0.0f, 0.0f},
  };
  vertices[4] = {
      math::vec3{-0.5f, -0.5f, 0.5f},
      math::vec2{1.0f, 0.0f},
      math::vec3{1.0f, 0.0f, 0.0f},
  };

  std::array<u32, 12> indices{
      0, 1, 2, 0, 3, 1, 0, 4, 3, 0, 2, 4,
  };

  auto vertexBuffer = backend.createBuffer({
      .debugName = "vertex",
      .usage = BufferUsage::VertexBuffer,
      .memoryUsage = MemoryUsage::GpuOnly,
      .deviceSize = sizeof(vertices[0]) * vertices.size(),
  });
  backend.writeBuffer({
      .buffer = vertexBuffer,
      .pData = vertices.data(),
      .deviceSize = sizeof(vertices),
      .offset = 0,
  });
  auto indexBuffer = backend.createBuffer({
      .debugName = "index",
      .usage = BufferUsage::IndexBuffer,
      .memoryUsage = MemoryUsage::GpuOnly,
      .deviceSize = sizeof(indices[0]) * indices.size(),
  });
  backend.writeBuffer({
      .buffer = indexBuffer,
      .pData = indices.data(),
      .deviceSize = sizeof(indices[0]) * indices.size(),
      .offset = 0,
  });

  Camera camera{
      .aspectRatio = platformCfg.window.width / (float)platformCfg.window.height,
  };
  // render loop
  CommandRecorder recorder;
  while (!platform.windowSystem().shouldClose()) {
    platform.processEvents();
    // begin frame
    recorder.clear();
    auto frameData = backend.beginFrame();
    backend.updateFrameConstants({
        .frameIndex = frameData.frameIndex,
        .projMat = camera.projection(),
        .viewMat = camera.view(),
    });
    recorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
    // record rendering commands
    // recorder.addCmd(ClearColorImageArgs{});
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(BindPipelineArgs{
        .pipeline = pipeline,
    });
    recorder.addCmd(BindGeometryArgs{
        .vertexBuffer = vertexBuffer,
        .indexBuffer = indexBuffer,
    });
    recorder.addCmd(SetScissorArgs{});

    recorder.addCmd(DrawIndexedArgs{
        .indexCount = static_cast<u32>(indices.size()),
        .instanceCount = 1,
        .firstIndex = 0,
        .vertexOffset = 0,
        .firstInstance = 0,
    });
    recorder.addCmd(EndRenderingArgs{});
    // end frame
    backend.endFrame(recorder);
  }

  backend.waitIdle();

  backend.destroyShaderModule(vertFragShader);
  // destroy resources
  backend.destroyGraphicsPipeline(pipeline);
  backend.destroyPipelineLayout(layout);
  backend.destroyBuffer(indexBuffer);
  backend.destroyBuffer(vertexBuffer);

  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}

TEST_CASE("test_textured_box", "[.]backend_vulkan") {
  using namespace pd;

  PlatformConfig platformCfg{
      .window =
          {
              .width = 1024,
              .height = 768,
          },
  };

  Platform platform(platformCfg);
  REQUIRE(platform.init());

  REQUIRE(platform.windowSystem().createWindow());

  BackendConfig backendCfg{
      .windowHandle = platform.windowSystem().nativeWindowHandle(),
      .width = platformCfg.window.width,
      .height = platformCfg.window.height,
      .enableDebug = true,
  };
  Backend backend;
  REQUIRE(backend.init(backendCfg));
  // test pipeline
  auto& fs = platform.fileSystem();

  AssetManager assetMgr{fs};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  pd::Asset::MetaInfo gltfAssetInfo{
      .name = "box",
      .path = ASSET_DIR "BoxTextured/BoxTextured.glb",
      .parseType = ParseType::Gltf,
  };

  Asset::MetaInfo shaderAssetInfo{
      .name = "pyramid shader",
      .path = ASSET_DIR "shader/pyramid/pyramid.slang",
      .parseType = ParseType::Shader,
      .shaderInfo =
          Asset::ShaderInfo{
              .moduleName = "pyramid",
              .modulePath = "pyramid/pyramid.slang",
          },
  };
  auto gltfAssetResult = assetMgr.createAsset(gltfAssetInfo);
  auto shaderAssetResult = assetMgr.createAsset(shaderAssetInfo);
  REQUIRE(gltfAssetResult);
  REQUIRE(shaderAssetResult);

  auto gltfAssetHandle = gltfAssetResult.value();
  auto shaderAssetHandle = shaderAssetResult.value();
  auto gltfAssetGetRes = assetMgr.getAsset(gltfAssetHandle);
  auto shaderAssetGetRes = assetMgr.getAsset(shaderAssetHandle);
  REQUIRE(gltfAssetGetRes);
  REQUIRE(shaderAssetGetRes);
  GltfAsset* gltfAsset = static_cast<GltfAsset*>(gltfAssetGetRes.value());
  ShaderAsset* shaderAsset = static_cast<ShaderAsset*>(shaderAssetGetRes.value());

  ResourceManager rscMgr(backend);

  auto gltfRefRes = rscMgr.registerResourceFromAsset<GltfAsset, PrefabResource_t>(ResourceCreateInfo<GltfAsset>{
      .pAsset = gltfAsset,
      .variantId = 0,
  });
  REQUIRE(gltfRefRes);

  auto gltfRef = gltfRefRes.value();
  auto gltfRefLoadRes = gltfRef.load();
  REQUIRE(gltfRefLoadRes);

  auto vertFragShader = backend.createShaderModule({
      .spirCode = std::as_bytes(std::span{shaderAsset->sources()}),
  });
  REQUIRE(vertFragShader);

  // pipeline layout
  auto layout = backend.createPipelineLayout({
      .debugName = "box layout",
      .pushConstantRanges = {PushConstantRange{
          .offset = 0,
          .size = sizeof(math::mat4),
          .shaderStages = {ShaderStage::Vertex},
      }},
  });
  REQUIRE(layout);

  // pipeline
  GraphicsPipelineCreateDesc pipelineDesc{
      .debugName = "box",
      .layout = layout,
      .vertexOptions =
          {
              .vertexLayouts = {{
                  .binding = 0,
                  .stride = sizeof(pd::Vertex),
              }},
              .attributes =
                  {
                      {.location = 0,
                       .binding = 0,
                       .format = VertexFormat::RGB32Sfloat,
                       .offset = offsetof(pd::Vertex, position)},
                      {.location = 1,
                       .binding = 0,
                       .format = VertexFormat::RG32Sfloat,
                       .offset = offsetof(pd::Vertex, uv)},
                      {.location = 2,
                       .binding = 0,
                       .format = VertexFormat::RGB32Sfloat,
                       .offset = offsetof(pd::Vertex, normal)},
                      {.location = 3,
                       .binding = 0,
                       .format = VertexFormat::RG32Sfloat,
                       .offset = offsetof(pd::Vertex, tangent)},
                  },
          },
  };
  pipelineDesc.shaderModules.push_back(vertFragShader);
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Vertex,
      .entryPoint = "vertMain",
  });
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Fragment,
      .entryPoint = "fragMain",
  });

  auto pipeline = backend.createGraphicsPipeline(pipelineDesc);
  REQUIRE(pipeline);

  Camera camera{
      .aspectRatio = platformCfg.window.width / (float)platformCfg.window.height,
  };
  auto meshes = gltfRef->meshes();
  // render loop
  CommandRecorder recorder;
  while (!platform.windowSystem().shouldClose()) {
    platform.processEvents();
    // begin frame
    recorder.clear();
    auto frameData = backend.beginFrame();
    backend.updateFrameConstants({
        .frameIndex = frameData.frameIndex,
        .projMat = camera.projection(),
        .viewMat = camera.view(),
    });
    recorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
    // record rendering commands
    // recorder.addCmd(ClearColorImageArgs{});
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(SetScissorArgs{});
    recorder.addCmd(BindPipelineArgs{
        .pipeline = pipeline,
    });
    for (int i = 0; i < meshes.size(); ++i) {
      recorder.addCmd(BindGeometryArgs{
          .vertexBuffer = meshes[i]->vertexBuffer()->buffer(),
          .indexBuffer = meshes[i]->indexBuffer()->buffer(),
      });
      auto submeshes = meshes[i]->submeshes();
      for (int j = 0; j < submeshes.size(); ++j) {
        recorder.addCmd(DrawIndexedArgs{
            .indexCount = submeshes[j].indexCount,
            .instanceCount = 1,
            .firstIndex = submeshes[j].firstIndex,
            .vertexOffset = static_cast<u32>(submeshes[j].vertexOffset),
            .firstInstance = 0,
        });
      }
    }
    recorder.addCmd(EndRenderingArgs{});
    // end frame
    backend.endFrame(recorder);
  }

  backend.waitIdle();

  // destroy resources
  backend.destroyShaderModule(vertFragShader);
  backend.destroyGraphicsPipeline(pipeline);
  backend.destroyPipelineLayout(layout);

  gltfRef.reset();
  REQUIRE(rscMgr.destroy());
  REQUIRE(assetMgr.destroy());
  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}