#pragma once

#include "pd/core/handle.hpp"
#include "pd/resource/resource.hpp"

namespace pd {
/**
 * @brief 资源句柄，只有ResourceManager能用，其他用户只能查看
 */
template <BaseOfResource T>
class ResourceHandle : public Handle {
 public:
  ResourceHandle() noexcept
      : Handle() {}

 private:
  friend class ResourceManager;

  explicit ResourceHandle(Handle::HandleId id) noexcept
      : Handle(id) {}
};

}  // namespace pd