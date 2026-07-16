#pragma once

#include <cassert>
#include <mutex>

#if defined(WIN32)
#include <malloc.h>
#endif

#include "tlsf/tlsf.h"

namespace pd {

namespace ptr {
// ensure pointer operates by bytes(mem address), not sizeof(T)
template <typename P, typename T>
static P* add(P* a, T b) noexcept {
  return (P*)(uintptr_t(a) + uintptr_t(b));
}

template <typename P>
static P* align(P* p, size_t alignment) noexcept {
  // alignment must be a power-of-two
  assert(alignment && !(alignment & alignment - 1));
  return (P*)((uintptr_t(p) + alignment - 1) & ~(alignment - 1));
}

template <typename P, typename T>
static size_t size(P* begin, T* end) noexcept {
  return uintptr_t(end) - uintptr_t(begin);
}
}  // namespace ptr

// allocate aligned memory
// ensure memory is aligned in different platform
inline void* aligned_alloc(size_t size, size_t align) noexcept {
  // for compatibility, align must larger than pointer's size
  align = (align < sizeof(void*)) ? sizeof(void*) : align;
  // align > 0 and must be power of 2
  assert(align && !(align & align - 1));
  // compatibility for POSIX
  assert((align % sizeof(void*)) == 0);
  void* p = nullptr;
  // use system native aligned malloc
#if defined(_WIN32)
  p = ::_aligned_malloc(size, align);
#else
  (void)::posix_memalign(&p, align, size);
#endif
  return p;
}

inline void aligned_free(void* p) noexcept {
#if defined(_WIN32)
  ::_aligned_free(p);
#else
  ::free(p);
#endif
}

namespace ResourceType {
/**
 * @brief 堆内存资源，需要指定内存资源范围时使用
 *
 * 目前采用tlsf分配算法，多预留了空间保证size是实际可用的size
 */
class HeapResource {
 public:
  HeapResource() noexcept = default;
  explicit HeapResource(size_t size) noexcept {
    if (size != 0) {
      auto actualSize = calculateSize(size);
      // 只分配足够内存，对齐由allocator负责
      mBegin = malloc(actualSize);
      mEnd = ptr::add(mBegin, actualSize);
    }
  }
  ~HeapResource() noexcept { free(mBegin); }

  HeapResource(const HeapResource&) = delete;
  HeapResource& operator=(const HeapResource&) = delete;
  HeapResource(HeapResource&&) noexcept = delete;
  HeapResource operator=(HeapResource&&) = delete;

  void* data() const noexcept { return mBegin; }

  void* begin() const noexcept { return mBegin; }

  void* end() const noexcept { return mEnd; }

  size_t size() const noexcept { return ptr::size(mBegin, mEnd); }

  friend void swap(HeapResource& lhs, HeapResource& rhs) noexcept {
    using std::swap;
    swap(lhs.mBegin, rhs.mBegin);
    swap(lhs.mEnd, rhs.mEnd);
  }

 private:
  void* mBegin = nullptr;
  void* mEnd = nullptr;

  size_t calculateSize(size_t rawSize) const noexcept {
    auto reservedSize = tlsf_size() + tlsf_pool_overhead();
    return rawSize + reservedSize;
  }
};

class NullResource {
 public:
  void* data() const noexcept { return nullptr; }
  size_t size() const noexcept { return 0; }
};
}  // namespace ResourceType

namespace PoolType {
class Default {};
/**
 * @brief TLSF算法内存分配器
 *
 * 默认的内存池算法
 *
 */
class Tlsf {
 public:
  Tlsf(void* begin, void* end) noexcept
      : mTlsfHandle(tlsf_create_with_pool(begin, ptr::size(begin, end))) {}

  Tlsf(const Tlsf&) = delete;
  Tlsf& operator=(const Tlsf&) = delete;
  Tlsf(Tlsf&&) noexcept = default;
  Tlsf& operator=(Tlsf&&) noexcept = default;
  ~Tlsf() { tlsf_destroy(mTlsfHandle); }

  void* alloc(size_t size, size_t alignment) noexcept {
    void* allocated = tlsf_memalign(mTlsfHandle, alignment, size);
    size_t actualSize = tlsf_block_size(allocated);
    mAllocatedSize += actualSize;
    return allocated;
  }

  void dealloc(void* p) noexcept {
    size_t actualSize = tlsf_block_size(p);
    mAllocatedSize -= actualSize;

    tlsf_free(mTlsfHandle, p);
  }

 private:
  void* mTlsfHandle = nullptr;
  size_t mAllocatedSize = 0;
};
/*
class FreeList {
 public:
  FreeList() noexcept = default;
  FreeList(void* begin, void* end, size_t elementSize, size_t alignment) noexcept {}

  FreeList(const FreeList&) = delete;
  FreeList& operator=(const FreeList&) = delete;
  FreeList(FreeList&&) noexcept = default;
  FreeList& operator=(FreeList&&) noexcept = default;

  void* pop() noexcept {}

  void push(void* p) noexcept {}

 private:
  struct Node {
    Node* next;
  };
  Node* mHead = nullptr;
};*/
}  // namespace PoolType

namespace AllocatorType {
// TODO(ryu613)
class LinearAllocator {};

/**
 * @brief 堆分配器(不是池)
 *
 */
class HeapAllocator {
 public:
  using pool_type = PoolType::Default;
  HeapAllocator() noexcept = default;
  template <typename RESOURCE>
  explicit HeapAllocator(const RESOURCE&) {}

  void* alloc(size_t size, size_t alignment = alignof(std::max_align_t)) noexcept {
    return aligned_alloc(size, alignment);
  }

  void free(void* p) noexcept { aligned_free(p); }

  void free(void* p, size_t size) noexcept { this->free(p); }

  ~HeapAllocator() noexcept = default;

  void swap(HeapAllocator& other) noexcept {}
};

/**
 * @brief 池分配器,默认使用tlsf实现
 *
 * 注：tlsf实现目前不是无状态的
 *
 * @tparam ElementSize 元素个数
 * @tparam Alignment 元素对齐要求
 * @tparam PoolType 池类型
 */
// clang-format off
template <size_t ElementSize,
          size_t Alignment = alignof(std::max_align_t), // 默认与平台最大对齐支持一致
          typename PoolType = PoolType::Tlsf>
// clang-format on
class PoolAllocator {
 public:
  using pool_type = PoolType;
  PoolAllocator(void* begin, void* end) noexcept
      : mPool(begin, end) {}
  // 不允许拷贝，因为只允许单一拥有权，允许移动(只是指针操作,默认即可)
  PoolAllocator(const PoolAllocator&) = delete;
  PoolAllocator& operator=(const PoolAllocator&) = delete;
  PoolAllocator(PoolAllocator&& rhs) noexcept = default;
  PoolAllocator& operator=(PoolAllocator&& rhs) noexcept = default;
  ~PoolAllocator() noexcept = default;

  template <typename Resource>
  explicit PoolAllocator(const Resource& resource) noexcept
      : PoolAllocator(resource.begin(), resource.end()) {}

  void* alloc(size_t size = ElementSize, size_t alignment = Alignment) noexcept {
    return mPool.alloc(size, alignment);
  }

  void free(void* p, size_t size = ElementSize) noexcept { mPool.dealloc(p); }

 private:
  PoolType mPool;
};
}  // namespace AllocatorType

// 锁策略
namespace LockingType {
/**
 * @brief 无锁策略，当lockguard时，实际无调用
 *
 */
class NoLock {
 public:
  void lock() noexcept {}
  void unlock() noexcept {}
};

// 互斥锁策略
using Mutex = std::mutex;
}  // namespace LockingType

namespace TrackingType {
class UnTrack {
 public:
  UnTrack() noexcept = default;
  UnTrack(const char* name, void* base, size_t size) noexcept {}

  void onAlloc(void* p, size_t size, size_t alignment) noexcept {}

  void onFree(void* p, size_t size = 0) noexcept {}
};

/**
 * @brief 用于调试时打印信息等
 *
 */
class Debug {
 public:
  Debug() noexcept = default;
  Debug(const char* name, void* begin, size_t size) noexcept
      : mName(name),
        mBegin(begin),
        mSize(size) {}

  void onAlloc(void* p, size_t size, size_t alignment) noexcept {}

  void onFree(void* p, size_t size = 0) noexcept {}

 private:
  const char* mName = nullptr;
  void* mBegin = nullptr;
  uint32_t mSize = 0;
};
}  // namespace TrackingType

/**
 * 基于模板策略的无状态内存分配器，支持多种分配策略，内存资源策略，加锁策略，追踪策略
 * AllocatorPolicy: HeapAllocator/LinearAllocator/...
 * ResourcePolicy: Null/Heap/...
 * LockingPolicy: NoLock/Mutex
 * TrackingPolicy: NoTrack/...
 */
// clang-format off
template <typename AllocatorPolicy,
          typename ResourcePolicy = ResourceType::HeapResource,
          typename LockingPolicy = LockingType::NoLock,
          typename TrackingPolicy = TrackingType::UnTrack>
// clang-format on
class Arena {
 public:
  Arena() noexcept = default;
  // 管理内存资源，不允许拷贝和移动
  Arena(const Arena&) noexcept = delete;
  Arena& operator=(const Arena&) noexcept = delete;

  // 指定大小的Arena，一般用于内存池
  template <typename... ARGS>
  Arena(const char* name, size_t size, ARGS&&... args)
      : mName(name),
        mResource(size),
        mAllocator(mResource, std::forward<ARGS>(args)...),
        mListener(name, mResource.data(), mResource.size()) {}

  // 制定策略的Arena，一般用于空或可转移的内存资源
  template <typename... ARGS>
  Arena(const char* name, ResourcePolicy&& resource, ARGS&&... args)
      : mName(name),
        mResource(std::forward<ResourcePolicy>(resource)),
        mAllocator(mResource, std::forward<ARGS>(args)...),
        mListener(name, mResource.data(), mResource.size()) {}

  /**
   * @brief Arena交换
   *
   * 作用:
   * 1. friend 满足对称性
   * 2. 高效交换，并使stl能够匹配此高效实现
   * 3. 方便可能的转移和延迟处理的需求，如异步回收
   *
   * @param lhs
   * @param rhs
   */
  friend void swap(Arena& lhs, Arena& rhs) noexcept {
    using std::swap;
    swap(lhs.mResource, rhs.mResource);
    swap(lhs.mAllocator, rhs.mAllocator);
    swap(lhs.mLock, rhs.mLock);
    swap(lhs.mListener, rhs.mListener);
    swap(lhs.mName, rhs.mName);
  }

  template <typename... ARGS>
  void* alloc(size_t size, size_t alignment, ARGS&&... args) noexcept {
    std::lock_guard<LockingPolicy> lock(mLock);
    void* p = mAllocator.alloc(size, alignment, std::forward<ARGS>(args)...);
    mListener.onAlloc(p, size, alignment);
    return p;
  }

  template <typename... ARGS>
  void free(void* p, size_t size, ARGS&&... args) noexcept {
    if (p) {
      std::lock_guard<LockingPolicy> lock(mLock);
      mListener.onFree(p, size);
      mAllocator.free(p, size, std::forward<ARGS>(args)...);
    }
  }

  /**
   * @brief 根据指定的分配策略和内存资源，分配并构造对象，支持自定义对齐
   *
   * @tparam T 构造的对象
   * @tparam ALIGN 对齐字节
   * @tparam ARGS 可变长构造参数列表
   * @param args 构造参数
   * @return T* 构造对象的指针
   */
  template <typename T, size_t ALIGN = alignof(T), typename... ARGS>
  T* make(ARGS&&... args) noexcept {
    void* p = alloc(sizeof(T), ALIGN);
    return p ? new (p) T(std::forward<ARGS>(args)...) : nullptr;
  }

  /**
   * @brief 销毁Arena中的对象
   *
   * @tparam T 对象类型
   * @param p 对象的指针
   */
  template <typename T>
  void destroy(T* p) noexcept {
    if (p) {
      p->~T();
      free((void*)p, sizeof(T));
    }
  }

  const char* getName() const noexcept { return mName; }

 private:
  const char* mName = nullptr;
  ResourcePolicy mResource;
  AllocatorPolicy mAllocator;
  LockingPolicy mLock;
  TrackingPolicy mListener;
};

}  // namespace pd