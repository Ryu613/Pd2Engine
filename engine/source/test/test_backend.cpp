#include "catch2/catch_test_macros.hpp"

#define BACKEND_VULKAN

#include "pd/platform/platform.hpp"
#include "pd/platform/platform_factory.hpp"
#include "pd/backend/backend.hpp"
#include "pd/backend/backend_factory.hpp"

TEST_CASE("test_backend_vulkan", "backend") {
  using namespace pd;
  // 初始化平台层
  IPlatform::Config config{
      .window =
          {
              .title = "PD2Engine_test",
              .width = 1024,
              .height = 768,
          },
  };
  auto platform = createPlatform(std::move(config));

  REQUIRE(platform->window()->create());

  // 初始化渲染后端
  auto* pWindow = platform->window();
  IBackend::Config backendConfig{
      .pWindow = pWindow,
  };
  auto vulkanBackend = createBackend(backendConfig);

  HwSwapchain hwSwapchain{
      .extent =
          {
              .width = 1024,
              .height = 768,
          },
  };
  auto handle = vulkanBackend->createSwapchain(hwSwapchain);
  REQUIRE(handle.isValid());

  vulkanBackend->destroySwapchain(handle);
}