#pragma once

#include "pd/api/api.hpp"

namespace pd {
class EngineApi : public Api {
 public:
  static EngineApi* create() noexcept;
  static void destroy(EngineApi* pEngine) noexcept;

 protected:
  EngineApi() noexcept = default;
  ~EngineApi() = default;
};
}  // namespace pd