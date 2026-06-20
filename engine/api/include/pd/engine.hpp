#pragma once

#include "pd/api/api.hpp"

namespace pd {
class Engine : public Api {
 public:
  static Engine* create() noexcept;

 protected:
  Engine() noexcept = default;
  ~Engine() = default;
};
}  // namespace pd