#pragma once

#include "pd/core/utils/handle.hpp"
#include "pd/resource/resource.hpp"

namespace pd {
/**
 * @brief 资源句柄，只有ResourceManager能用，其他用户只能查看
 */
// template <BaseOfResource T>
// class ResourceHandle : public BaseHandle {
//  public:
//   ResourceHandle() noexcept = default;

//  private:
//   friend class ResourceManager;

//   explicit ResourceHandle(Handle::HandleId id, uint32_t gen) noexcept
//       : BaseHandle(id, gen) {}
// };

template <typename T>
using ResourceHandle = TypedHandle<T>;

}  // namespace pd