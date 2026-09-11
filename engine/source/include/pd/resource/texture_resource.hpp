#pragma once

#include "pd/resource/resource.hpp"

namespace pd {
class TextureResource : public Resource {
 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;
};
}  // namespace pd