#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class PrefabResource : public Resource {
 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;
};
}  // namespace pd