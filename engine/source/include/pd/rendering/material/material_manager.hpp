#pragma once

#include "pd/rendering/material/material_types.hpp"
#include "pd/rendering/material/material_definition.hpp"
#include "pd/rendering/material/material_instance.hpp"

#include "pd/core/utils/map.hpp"
#include "pd/core/utils/hash.hpp"

namespace pd {
class MaterialManager {
 public:
  using IdType = MaterialDefinition::IdType;

  MaterialManager() = default;
  ~MaterialManager() = default;
  DELETE_COPY_MOVE(MaterialManager);

  Result<void> init() noexcept { return {}; }
  Result<void> destroy() noexcept { return {}; }

  IdType registerMaterial(MaterialDefinition&& def) {
    // 1. 获取名称, 在nameToId查询是否存在
    // 2. 判断存在
    // 2.1. 若存在，直接返回这个id
    // 2.2. 若不存在, 执行注册
    // 3. 注册: 生成id, 把数据搬到definition data里
    // 3.1. 更新name to id表
    return {};
  }

  IdType createInstance(IdType definitionId) {
    // 1. 在definition data里看是否存在
    // 2. 若存在，需要对instance判重吗?感觉不需要
    return {};
  }

 private:
  struct PipelineKey {
    IdType definitionId;    // 64
    u32 pipelineStateFlag;  // 32
    u32 padding = 0U;       // to avoid hasher use undefined padding value

    bool operator==(const PipelineKey& other) const noexcept = default;
  };
  struct PipelineKeyHasher {
    size_t operator()(const PipelineKey& k) const noexcept { return rapidhash(&k, sizeof(k)); }
  };

  // id找对象
  template <typename T>
  using Data = util::RobinMap<IdType, std::unique_ptr<T>>;

  Data<MaterialDefinition> mDefinitions;
  Data<MaterialInstance> mInstances;

  // name找id
  util::RobinMap<std::string, IdType, util::StringHasher> mNameToId;

  // 通过pipeline某些特征生成的key来找对应的pipeline handle
  util::RobinMap<PipelineKey, BackendHandle<Pipeline_t>, PipelineKeyHasher> mPipelineHandles;

  IdType nextDefinitionId = 1;
  IdType nextInstanceId = 1;

  IdType nextId(IdType& currentId) { return ++currentId; }
};
}  // namespace pd