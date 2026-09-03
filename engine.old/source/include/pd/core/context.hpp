#pragma once

#include "pd/core/allocators.hpp"

namespace pd {

struct EngineContext {
  // ILogger* logger = nullptr;
  GlobalAllocator* gAllocator = nullptr;
};

namespace global {
inline std::unique_ptr<EngineContext>& ContextStorage() noexcept {
  static std::unique_ptr<EngineContext> context;
  return context;
}

inline void initContext(GlobalAllocator& allocator) noexcept {
  static std::once_flag flag;
  std::call_once(flag, [&allocator]() {
    ContextStorage() = std::make_unique<EngineContext>();
    ContextStorage()->gAllocator = &allocator;
  });
}

inline const EngineContext& getContext() noexcept { return *ContextStorage(); }

inline void destroyContext() noexcept { ContextStorage().reset(); }
}  // namespace global

}  // namespace pd