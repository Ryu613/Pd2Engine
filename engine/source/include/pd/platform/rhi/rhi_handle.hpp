#pragma once

#include "pd/core/handle.hpp"
#include "pd/platform/rhi/rhi_resource.hpp"

namespace pd {
template <DerivedRhiResource T>
class RhiHandle : public Handle {
 private:
  friend class VulkanResourceManager;
  explicit RhiHandle(HandleId id)
      : Handle(id) {}
};
}  // namespace pd