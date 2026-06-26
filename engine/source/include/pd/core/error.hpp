#pragma once

namespace pd {
struct Error {
  enum class Engine : uint8_t {
    InitializeFailed,
    RunFailed,
    ResourceGCFailed,
  };
  enum class Layer : uint8_t {
    LayerNotFound,
    LayerAttachFailed,
    LayerUpdateFailed,
    LayerDetachFailed,
  };
  enum class Scene : uint8_t {
    SceneLoadFailed,
    SceneUpdateFailed,
    SceneUnloadFailed,
  };
  enum class Rendering : uint8_t {
    SwapchainOutDate,
    Timeout,
    NotImplemented,
  };
};
}  // namespace pd