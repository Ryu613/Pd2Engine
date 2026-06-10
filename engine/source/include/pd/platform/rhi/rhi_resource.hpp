#pragma once

namespace pd {
struct RhiResource {};

struct RhiTexture : public RhiResource {};

template <typename T>
concept DerivedRhiResource = std::derived_from<T, RhiResource>;
}  // namespace pd