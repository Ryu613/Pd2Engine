#include "catch2/catch_test_macros.hpp"
#include "pd/resource/resource_manager.hpp"

TEST_CASE("test_resource_manager", "resource_manager") {
  using namespace pd;
  auto mgr = std::make_unique<ResourceManager>();
  TextureResource::Properties props{
      .name = "test_texture",
      .path = "asset/texture/test.ktx",
  };
  TextureResource tex{props};
  auto handle = mgr->registerResource<TextureResource_t>(std::move(tex));
  REQUIRE(handle.isValid());

  // 重复注册
  TextureResource tex1{props};
  auto handle1 = mgr->registerResource<TextureResource_t>(std::move(tex1));

  REQUIRE(handle == handle1);

  auto exist = mgr->hasResource<TextureResource_t, TextureResource>(props.path);
  REQUIRE(exist);
}