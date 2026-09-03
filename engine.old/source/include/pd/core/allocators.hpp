#pragma once

#include "pd/core/utils/memory.hpp"

namespace pd {
// debug模式为强制同步并且可trace
#ifndef NDEBUG
using GlobalAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                              LockingType::Mutex, TrackingType::Debug>;
template <size_t ElemSize, size_t Align>
using PoolAllocator =
    Arena<AllocatorType::PoolAllocator<ElemSize, Align, PoolType::Tlsf>,
          ResourceType::HeapResource, LockingType::Mutex, TrackingType::Debug>;
#else
using GlobalAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                              LockingType::Nolock, TrackingType::UnTrack>;
template <size_t ElemSize, size_t Align>
using PoolAllocator =
    Arena<AllocatorType::PoolAllocator<ElemSize, Align, PoolType::Tlsf>,
          ResourceType::HeapResource, LockingType::Nolock, TrackingType::UnTrack>;
#endif

}  // namespace pd