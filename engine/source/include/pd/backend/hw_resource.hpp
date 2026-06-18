#pragma once

namespace pd {
class HwResource {};

template <typename T>
concept BaseOfHwResource = std::derived_from<T, HwResource>;
}  // namespace pd