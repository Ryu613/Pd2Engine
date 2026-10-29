#pragma once

#include "tsl/robin_map.h"
#include "pd/core/utils/hash.hpp"

namespace pd {
namespace util {

template <typename Key, typename T, typename Hasher = std::hash<Key>>
using RobinMap = tsl::robin_map<Key, T, Hasher, std::equal_to<Key>, std::allocator<std::pair<Key, T>>, true>;
}  // namespace util
}  // namespace pd