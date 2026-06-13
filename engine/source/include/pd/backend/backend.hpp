#pragma once

namespace pd {
class Backend {
 public:
  struct Config {};
  Backend() noexcept = default;
  virtual ~Backend() = default;
  MOVABLE_ONLY(Backend);
};
}  // namespace pd