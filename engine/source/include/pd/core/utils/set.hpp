#pragma once

#include "tsl/robin_set.h"
#include "pd/core/utils/hash.hpp"

namespace pd::util {

template <typename Key, typename Hasher = std::hash<Key>>
using RobinSet = tsl::robin_set<Key, Hasher, std::equal_to<Key>, std::allocator<Key>, true>;
}