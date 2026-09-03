#pragma once

#include "rapidhash/rapidhash.h"

namespace pd::util {
static u64 hashString(std::string_view path) noexcept {
  return rapidhash(path.data(), path.size());
}
}  // namespace pd::util