#pragma once

#include "pd/core/utils/handle.hpp"
#include "pd/backend/hw_resource.hpp"

namespace pd {

template <BaseOfHwResource THwResource>
using HwHandle = TypedHandle<THwResource>;
}