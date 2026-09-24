#include "catch2/catch_test_macros.hpp"

#include "pd/core/config.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"
#include "pd/backend/command_recorder.hpp"
#include "pd/asset/compiler/shader_compiler.hpp"

#include "pd/core/math/math.hpp"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

TEST_CASE("core_render_cmds", "backend_vulkan") {
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

  GraphicsPipelineCreateDesc pipelineDesc{
      .debugName = "triangle",
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

  // buffers
  // front face color is red!
  std::array<Vertex, 5> vertices;
  vertices[0] = {
      math::vec3{0.0f, 0.5f, 0.0f},
      math::vec3{1.0f, 0.0f, 0.0f},
      math::vec2{0.0f, 0.0f},
  };
  vertices[1] = {
      math::vec3{-0.5f, -0.5f, -0.5f},
      math::vec3{0.0f, 1.0f, 1.0f},
      math::vec2{0.0f, 1.0f},
  };
  vertices[2] = {
      math::vec3{0.5f, -0.5f, -0.5f},
      math::vec3{0.0f, 0.0f, 1.0f},
      math::vec2{1.0f, 1.0f},
  };
  vertices[3] = {
      math::vec3{0.5f, -0.5f, 0.5f},
      math::vec3{1.0f, 0.0f, 0.0f},
      math::vec2{1.0f, 1.0},
  };
  vertices[4] = {
      math::vec3{-0.5f, -0.5f, 0.5f},
      math::vec3{1.0f, 0.0f, 0.0f},
      math::vec2{1.0f, 0.0f},
  };

  std::array<u32, 12> indices{
      0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
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

  // pipeline data contains pipeline, layout handles, and other infos
  auto pipelineData = backend.createGraphicsPipeline(pipelineDesc);
  // render loop
  CommandRecorder recorder;
  while (!platform.windowSystem().shouldClose()) {
    platform.processEvents();
    // begin frame
    recorder.clear();
    auto frameData = backend.beginFrame();
    recorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
    // record rendering commands
    // recorder.addCmd(ClearColorImageArgs{});
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(BindPipelineArgs{
        .vertexBuffer = vertexBuffer,
        .indexBuffer = indexBuffer,
        .pipeline = pipelineData.pipeline,
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
  backend.destroyGraphicsPipeline({pipelineData.layout, pipelineData.pipeline});
  backend.destroyBuffer(indexBuffer);
  backend.destroyBuffer(vertexBuffer);

  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}
