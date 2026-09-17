#include "catch2/catch_test_macros.hpp"

#include "pd/core/config.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"
#include "pd/backend/command_recorder.hpp"
#include "pd/rendering/shader/shader_manager.hpp"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

TEST_CASE("core_cmds", "backend_vulkan") {
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
  auto shaderCode = fs.readFileBinary(ASSET_DIR "shader/pyramid/pyramid.slang");
  // return spir-v code, and reflect info
  ShaderManager shaderManager;
  auto shaderDataRes = shaderManager.compile({
      .code = shaderCode,
  });
  REQUIRE(shaderDataRes);
  auto& shaderData = shaderDataRes.value();
  GraphicsPipelineDesc pipelineDesc{
      .debugName = "triangle",
  };
  pipelineDesc.shaderDatas.push_back(shaderData);
  pipelineDesc.shaderPrograms.push_back({
      .shaderCodeIndex = pipelineDesc.shaderDatas.size() - 1,
      .stage = ShaderStage::Vertex,
      .entryPoint = "vertex",
  });
  pipelineDesc.shaderPrograms.push_back({
      .shaderCodeIndex = pipelineDesc.shaderDatas.size() - 1,
      .stage = ShaderStage::Fragment,
      .entryPoint = "fragment",
  });

  // pipeline data contains pipeline, layout handles, and other infos
  auto pipelineData = backend.createGraphicsPipeline(pipelineDesc);
  // render loop
  CommandRecorder recorder;
  while (!platform.windowSystem().shouldClose()) {
    // begin frame
    recorder.clear();
    auto frameData = backend.beginFrame();
    recorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
    // record rendering commands
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(SetScissorArgs{});
    recorder.addCmd(BindPipelineArgs{
        .pipeline = pipelineData.pipeline,
    });
    recorder.addCmd(EndRenderingArgs{});
    // end frame
    backend.endFrame(recorder);
  }
}