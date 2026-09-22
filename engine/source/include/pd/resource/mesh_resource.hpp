#pragma once

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class MeshResource : public Resource {
 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;
};
}  // namespace pd