#pragma once

namespace pd {
struct HwResource {};

template <typename T>
concept BaseOfHwResource = std::derived_from<T, HwResource>;
}  // namespace pd