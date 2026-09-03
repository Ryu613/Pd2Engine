#pragma once

namespace pd {
// numbers
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

inline constexpr u64 u64_max = UINT64_MAX;
inline constexpr u32 u32_max = UINT32_MAX;
inline constexpr u16 u16_max = UINT16_MAX;
inline constexpr u8 u8_max = UINT8_MAX;
inline constexpr i64 i64_max = INT64_MAX;
inline constexpr i32 i32_max = INT32_MAX;
inline constexpr i16 i16_max = INT16_MAX;
inline constexpr i8 i8_max = INT8_MAX;

// string
using cstr = const char*;

// enums
enum class BackendType : u8 {
  Noop = 0,
  Vulkan = 1,
  D3D12 = 2,
  Unknown = u8_max,
};

enum class WindowType : u8 {
  Win32 = 1,
  SDL3,
  Wayland,
  Android,
  Unknown = u8_max,
};

// containers
// template <typename T>
// using vector = std::pmr::vector<T>;
}  // namespace pd