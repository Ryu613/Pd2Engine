#pragma once

namespace pd {
// TODO(author): not tested!
template <typename T, typename Arena>
struct ArenaDeleter {
  Arena* arena;

  void operator()(T* ptr) const noexcept {
    if (ptr) {
      arena->destroy(ptr);
    }
  }
};
template <typename T, typename Arena>
using uptr = std::unique_ptr<T, ArenaDeleter<T, Arena>>;

template <typename T, typename Arena, typename... Args>
[[nodiscard]] inline uptr<T, Arena> make_uptr(Arena& arena, Args&&... args) {
  T* t = arena.make<T>(std::forward<Args>(args)...);

  return uptr<T, Arena>(t, ArenaDeleter<T, Arena>{&arena});
}
}  // namespace pd