#include "catch2/catch_test_macros.hpp"
#include "pd/core/utils/memory.hpp"
#include "pd/core/utils/vector.hpp"
#include "pd/core/utils/hash.hpp"

TEST_CASE("test_vector", "container") {
  using namespace pd;
  using HeapAllocator = Arena<AllocatorType::HeapAllocator, ResourceType::NullResource,
                              LockingType::Mutex, TrackingType::Debug>;
  HeapAllocator allocator;
  //   util::FixedCapacityVector<int, 64, HeapAllocator> vec{allocator};
}

TEST_CASE("test_hash", "utils") {
  std::string path = "aaa/bbb/ccc.gltf";
  auto hashed = pd::util::hashString(path);
}