#pragma once

#include "rapidhash/rapidhash.h"

namespace pd::util {
inline static u64 hashString(std::string_view str) noexcept {
  return rapidhash(str.data(), str.size());
}
}  // namespace pd::util