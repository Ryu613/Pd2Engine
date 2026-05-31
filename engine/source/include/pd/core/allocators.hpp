#pragma once

#include "pd/core/utils/memory.hpp"

namespace pd {
// debug模式为强制同步并且可trace
#ifndef NDEBUG
using HeapAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                            LockingType::Mutex, TrackingType::Debug>;
#else
using HeapAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                            LockingType::Nolock, TrackingType::UnTrack>;
#endif
}  // namespace pd