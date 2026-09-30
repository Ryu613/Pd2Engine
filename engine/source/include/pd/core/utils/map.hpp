#pragma once

#include "tsl/robin_map.h"
#include "pd/core/utils/hash.hpp"

namespace pd {
namespace util {
struct StringHasher {
  using is_transparent = void;
  size_t operator()(const std::string& key) const noexcept {
    return static_cast<size_t>(rapidhash(key.data(), key.size()));
  }
  size_t operator()(std::string_view key) const noexcept {
    return static_cast<size_t>(rapidhash(key.data(), key.size()));
  }
};

template <typename Key, typename T, typename Hasher = std::hash<Key>>
using RobinMap = tsl::robin_map<Key, T, Hasher, std::equal_to<Key>, std::allocator<std::pair<Key, T>>, true>;
}  // namespace util
}  // namespace pd