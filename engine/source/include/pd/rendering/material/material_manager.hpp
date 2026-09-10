#pragma once

#include "pd/rendering/material/material_types.hpp"
#include "pd/rendering/material/material_definition.hpp"
#include "pd/rendering/material/material_instance.hpp"

#include "pd/core/utils/map.hpp"
#include "pd/core/utils/hash.hpp"

namespace pd {
class Backend;
class MaterialManager {
 public:
  using IdType = MaterialDefinition::IdType;

  explicit MaterialManager(Backend* backend);
  ~MaterialManager() = default;
  DELETE_COPY_MOVE(MaterialManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  IdType registerMaterial(MaterialDefinition&& def);

  IdType createInstance(IdType definitionId);

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

  Backend* mBackend = nullptr;

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