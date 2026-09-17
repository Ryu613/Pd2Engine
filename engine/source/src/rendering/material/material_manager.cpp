#include "pd/rendering/material/material_manager.hpp"

namespace pd {
using IdType = MaterialManager::IdType;

MaterialManager::MaterialManager(Backend* backend)
    : mBackend(backend) {}

Result<void> MaterialManager::init() noexcept { return {}; }
Result<void> MaterialManager::destroy() noexcept { return {}; }

IdType MaterialManager::registerMaterial(MaterialDefinition&& def) {
  // 1. 获取名称, 在nameToId查询是否存在
  // 2. 判断存在
  // 2.1. 若存在，直接返回这个id
  // 2.2. 若不存在, 执行注册
  // 3. 注册: 生成id, 把数据搬到definition data里
  // 3.1. 更新name to id表
  return {};
}

IdType MaterialManager::createInstance(IdType definitionId) {
  // 1. 在definition data里看是否存在
  // 2. 若存在，需要对instance判重吗?感觉不需要
  return {};
}
}  // namespace pd