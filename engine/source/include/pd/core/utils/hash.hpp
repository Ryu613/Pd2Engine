#pragma once

#include "rapidhash/rapidhash.h"

namespace pd::util {
inline static u64 hashString(std::string_view str) noexcept { return rapidhash(str.data(), str.size()); }

struct StringHasher {
  using is_transparent = void;
  size_t operator()(const std::string& key) const noexcept {
    return static_cast<size_t>(rapidhash(key.data(), key.size()));
  }
  size_t operator()(std::string_view key) const noexcept {
    return static_cast<size_t>(rapidhash(key.data(), key.size()));
  }
};
}  // namespace pd::util