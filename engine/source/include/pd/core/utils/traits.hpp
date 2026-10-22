#pragma once

namespace pd::traits {

// 限制类型为派生类且排除基类
// 若需要基类，可直接derived_from
template <typename T, typename Base>
concept DerivedWithoutBase = std::derived_from<std::remove_cvref_t<T>, std::remove_cvref_t<Base>> &&
                              !std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<Base>>;
}  // namespace pd