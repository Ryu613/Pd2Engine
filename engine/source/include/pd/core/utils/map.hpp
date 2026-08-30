#pragma once

#include "tsl/robin_map.h"
#include "pd/core/utils/hash.hpp"

namespace pd {
namespace util {
struct StringHasher {
  size_t operator()(const std::string& key) const noexcept {
    return static_cast<size_t>(rapidhash(key.data(), key.size()));
  }
};

template <typename Key, typename T, typename Hasher>
using RobinMap = tsl::robin_map<Key, T, Hasher>;
}  // namespace util
}  // namespace pd