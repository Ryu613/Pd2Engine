#include "catch2/catch_test_macros.hpp"

#include "pd/core/utils/memory.hpp"

class alignas(64) NoneT {
 public:
  explicit NoneT(std::string a) : a{a} {}
  ~NoneT() { std::cout << "destroy NoneT\n"; }

  std::string getA() const { return a; }

 private:
  std::string a;
};

TEST_CASE("test_heap_alloc", "arena") {
  using namespace pd;
  using HeapAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                              LockingType::Mutex, TrackingType::Debug>;
  // dynamic trivial object allocation(heap)
  HeapAllocator allocator{"test_arena", ResourceType::NullResource{}};
  auto* p = allocator.make<int>(6);
  allocator.destroy(p);

  // dynamic none-trivial and over-aligned object allocation(heap)
  std::string aValue = "ABC";
  auto* noneTPtr = allocator.make<NoneT>(aValue);
  auto& noneT = *noneTPtr;
  std::cout << noneT.getA() << "\n"
            << "alignof(T): " << alignof(NoneT) << "\n"
            << "sizeof(T): " << sizeof(noneT) << "\n";
  allocator.destroy(noneTPtr);
}

TEST_CASE("test_pool", "arena") {
  using namespace pd;
  // noneT object pool
  using Pool = Arena<AllocatorType::PoolAllocator<sizeof(NoneT)>,
                     ResourceType::HeapResource, LockingType::Mutex, TrackingType::Debug>;
  size_t poolSize = 32 * 1024 * 1024;  // 32M
  Pool allocator{"test_arena", poolSize};
  // trivial
  // auto* p = allocator.make<int>(6);
  // allocator.destroy(p);

  // // none trivial
  std::string aValue = "ABC";
  // auto* noneTPtr = allocator.make<NoneT>(aValue);
  // auto& noneT = *noneTPtr;
  // std::cout << noneT.getA() << "\n"
  //           << "alignof(T): " << alignof(NoneT) << "\n"
  //           << "sizeof(T): " << sizeof(noneT) << "\n";
  // allocator.destroy(noneTPtr);

  // create and free large number of objects
  std::vector<NoneT*> noneTList;
  noneTList.reserve(1000);
  for (size_t i = 0; i < 1000; ++i) {
    auto* p = allocator.make<NoneT>(aValue);
    noneTList.emplace_back(p);
  }
  // destroy one of them
  auto it = noneTList.begin() + 311;
  noneTList.erase(it);

  allocator.free(noneTList[0], noneTList.size());
}